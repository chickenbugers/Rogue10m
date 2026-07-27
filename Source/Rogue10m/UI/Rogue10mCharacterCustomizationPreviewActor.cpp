// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacterCustomizationPreviewActor.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Rogue10mCharacterCustomizationDataAsset.h"

namespace
{
	constexpr float CustomizationPreviewStagingHeight = 75000.0f;
	constexpr float CustomizationPreviewFieldOfView = 30.0f;
	constexpr float CustomizationPreviewFrameMargin = 1.18f;
}

ARogue10mCharacterCustomizationPreviewActor::ARogue10mCharacterCustomizationPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	SetActorEnableCollision(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	PreviewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Preview Root"));
	PreviewRoot->SetupAttachment(SceneRoot);

	auto ConfigureMesh = [this](USkeletalMeshComponent* Mesh)
	{
		Mesh->SetupAttachment(PreviewRoot);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCastShadow(false);
		Mesh->SetReceivesDecals(false);
		Mesh->SetVisibleInSceneCaptureOnly(true);
		Mesh->SetComponentTickEnabled(false);
	};

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body Mesh"));
	ConfigureMesh(BodyMesh);
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair Mesh"));
	ConfigureMesh(HairMesh);
	FacialMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Facial Mesh"));
	ConfigureMesh(FacialMesh);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Character Capture"));
	SceneCapture->SetupAttachment(SceneRoot);
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->FOVAngle = CustomizationPreviewFieldOfView;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->MaxViewDistanceOverride = 2000.0f;
	SceneCapture->ShowFlags.SetAtmosphere(false);
	SceneCapture->ShowFlags.SetFog(false);
	SceneCapture->ShowFlags.SetVolumetricFog(false);
	SceneCapture->ShowFlags.SetMotionBlur(false);
	SceneCapture->ShowFlags.SetBloom(false);

	KeyLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Key Light"));
	KeyLight->SetupAttachment(SceneRoot);
	KeyLight->SetRelativeLocation(FVector(190.0f, 150.0f, 230.0f));
	KeyLight->SetIntensity(5000.0f);
	KeyLight->SetLightColor(FLinearColor(1.0f, 0.82f, 0.66f));
	KeyLight->SetAttenuationRadius(750.0f);
	KeyLight->SetCastShadows(false);

	FillLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Fill Light"));
	FillLight->SetupAttachment(SceneRoot);
	FillLight->SetRelativeLocation(FVector(100.0f, -230.0f, 130.0f));
	FillLight->SetIntensity(2600.0f);
	FillLight->SetLightColor(FLinearColor(0.38f, 0.58f, 1.0f));
	FillLight->SetAttenuationRadius(700.0f);
	FillLight->SetCastShadows(false);

	RimLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Rim Light"));
	RimLight->SetupAttachment(SceneRoot);
	RimLight->SetRelativeLocation(FVector(-170.0f, 20.0f, 210.0f));
	RimLight->SetIntensity(3600.0f);
	RimLight->SetLightColor(FLinearColor(0.48f, 0.68f, 1.0f));
	RimLight->SetAttenuationRadius(700.0f);
	RimLight->SetCastShadows(false);

	SceneCapture->ShowOnlyComponent(BodyMesh);
	SceneCapture->ShowOnlyComponent(HairMesh);
	SceneCapture->ShowOnlyComponent(FacialMesh);
}

bool ARogue10mCharacterCustomizationPreviewActor::InitializePreview(
	URogue10mCharacterCustomizationDataAsset* InCatalog, FIntPoint Resolution)
{
	if (!InCatalog)
	{
		return false;
	}

	Catalog = InCatalog;
	SetActorLocation(FVector(0.0f, 0.0f, CustomizationPreviewStagingHeight));

	RenderTarget = NewObject<UTextureRenderTarget2D>(
		this, TEXT("CharacterCustomizationRenderTarget"), RF_Transient);
	if (!RenderTarget)
	{
		return false;
	}

	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
	RenderTarget->ClearColor = FLinearColor(0.008f, 0.012f, 0.02f, 0.0f);
	RenderTarget->InitAutoFormat(
		FMath::Clamp(Resolution.X, 256, 2048),
		FMath::Clamp(Resolution.Y, 256, 2048));
	RenderTarget->UpdateResourceImmediate(true);
	SceneCapture->TextureTarget = RenderTarget;
	return true;
}

bool ARogue10mCharacterCustomizationPreviewActor::SetAppearance(
	const FRogue10mCharacterAppearance& Appearance)
{
	if (!Catalog || !Catalog->ApplyAppearance(
		Appearance, BodyMesh, HairMesh, FacialMesh))
	{
		return false;
	}

	PreviewRoot->SetRelativeRotation(FRotator(0.0f, -18.0f, 0.0f));
	FrameCharacter();
	CapturePreview();
	return true;
}

void ARogue10mCharacterCustomizationPreviewActor::AddPreviewYaw(float DeltaYaw)
{
	if (!PreviewRoot || FMath::IsNearlyZero(DeltaYaw))
	{
		return;
	}
	PreviewRoot->AddLocalRotation(FRotator(0.0f, DeltaYaw, 0.0f));
	CapturePreview();
}

void ARogue10mCharacterCustomizationPreviewActor::SetPreviewActive(bool bActive)
{
	BodyMesh->SetVisibility(bActive, false);
	HairMesh->SetVisibility(bActive && HairMesh->GetSkeletalMeshAsset(), false);
	FacialMesh->SetVisibility(bActive && FacialMesh->GetSkeletalMeshAsset(), false);
	KeyLight->SetVisibility(bActive);
	FillLight->SetVisibility(bActive);
	RimLight->SetVisibility(bActive);
	if (bActive)
	{
		CapturePreview();
	}
}

void ARogue10mCharacterCustomizationPreviewActor::FrameCharacter()
{
	FBox Bounds(ForceInit);
	for (USkeletalMeshComponent* Mesh : { BodyMesh.Get(), HairMesh.Get(), FacialMesh.Get() })
	{
		if (Mesh && Mesh->GetSkeletalMeshAsset() && Mesh->IsVisible())
		{
			Mesh->UpdateBounds();
			Bounds += Mesh->Bounds.GetBox();
		}
	}
	if (!Bounds.IsValid)
	{
		return;
	}

	const FVector Center = GetActorTransform().InverseTransformPosition(Bounds.GetCenter());
	const FVector Extent = Bounds.GetExtent();
	const float HalfFov = FMath::DegreesToRadians(SceneCapture->FOVAngle * 0.5f);
	const float Distance = FMath::Max(
		190.0f,
		(Extent.Z / FMath::Max(0.1f, FMath::Tan(HalfFov))) * CustomizationPreviewFrameMargin);
	const FVector CameraLocation(Distance, Center.Y, Center.Z);
	SceneCapture->SetRelativeLocation(CameraLocation);
	SceneCapture->SetRelativeRotation((Center - CameraLocation).Rotation());
}

void ARogue10mCharacterCustomizationPreviewActor::CapturePreview()
{
	if (SceneCapture && SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
	}
}
