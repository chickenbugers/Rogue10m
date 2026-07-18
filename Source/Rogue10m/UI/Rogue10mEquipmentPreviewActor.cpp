// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mEquipmentPreviewActor.h"

#include "Character/Rogue10mCharacter.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"

namespace
{
	constexpr float PreviewStagingHeight = 50000.0f;
	constexpr float PreviewFieldOfView = 32.0f;
	constexpr float PreviewFrameMargin = 1.2f;
	constexpr int32 MinimumRenderTargetSize = 128;
	constexpr int32 MaximumRenderTargetSize = 2048;
}

ARogue10mEquipmentPreviewActor::ARogue10mEquipmentPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	SetActorEnableCollision(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Character Capture"));
	SceneCapture->SetupAttachment(SceneRoot);
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->FOVAngle = PreviewFieldOfView;
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
	KeyLight->SetRelativeLocation(FVector(180.0f, 140.0f, 220.0f));
	KeyLight->SetIntensity(4200.0f);
	KeyLight->SetLightColor(FLinearColor(1.0f, 0.88f, 0.72f));
	KeyLight->SetAttenuationRadius(700.0f);
	KeyLight->SetCastShadows(false);

	FillLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Fill Light"));
	FillLight->SetupAttachment(SceneRoot);
	FillLight->SetRelativeLocation(FVector(80.0f, -220.0f, 120.0f));
	FillLight->SetIntensity(2200.0f);
	FillLight->SetLightColor(FLinearColor(0.45f, 0.62f, 1.0f));
	FillLight->SetAttenuationRadius(650.0f);
	FillLight->SetCastShadows(false);

	RimLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Rim Light"));
	RimLight->SetupAttachment(SceneRoot);
	RimLight->SetRelativeLocation(FVector(-180.0f, 40.0f, 180.0f));
	RimLight->SetIntensity(3000.0f);
	RimLight->SetLightColor(FLinearColor(0.42f, 0.58f, 1.0f));
	RimLight->SetAttenuationRadius(650.0f);
	RimLight->SetCastShadows(false);
}

bool ARogue10mEquipmentPreviewActor::InitializePreview(ACharacter* InSourceCharacter, FIntPoint InResolution)
{
	if (!InSourceCharacter)
	{
		return false;
	}

	SourceCharacter = InSourceCharacter;
	SetActorLocation(InSourceCharacter->GetActorLocation() + FVector(0.0f, 0.0f, PreviewStagingHeight));

	const int32 Width = FMath::Clamp(InResolution.X, MinimumRenderTargetSize, MaximumRenderTargetSize);
	const int32 Height = FMath::Clamp(InResolution.Y, MinimumRenderTargetSize, MaximumRenderTargetSize);
	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("EquipmentPreviewRenderTarget"), RF_Transient);
	if (!RenderTarget)
	{
		return false;
	}

	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
	RenderTarget->ClearColor = FLinearColor(0.012f, 0.016f, 0.022f, 0.0f);
	RenderTarget->InitAutoFormat(Width, Height);
	RenderTarget->UpdateResourceImmediate(true);
	SceneCapture->TextureTarget = RenderTarget;

	return RefreshFromCharacter();
}

bool ARogue10mEquipmentPreviewActor::RefreshFromCharacter()
{
	ACharacter* Character = SourceCharacter.Get();
	if (!Character || !SceneCapture)
	{
		return false;
	}

	ClearPreviewMeshes();

	TArray<USkeletalMeshComponent*> SourceMeshes;
	Character->GetComponents(SourceMeshes);
	const ARogue10mCharacter* RogueCharacter = Cast<ARogue10mCharacter>(Character);
	USkeletalMeshComponent* FirstPersonMesh = RogueCharacter ? RogueCharacter->GetFirstPersonMesh() : nullptr;

	for (USkeletalMeshComponent* SourceMesh : SourceMeshes)
	{
		if (!SourceMesh || SourceMesh == FirstPersonMesh || !SourceMesh->GetSkeletalMeshAsset())
		{
			continue;
		}

		USkeletalMeshComponent* PreviewMesh = CreatePreviewMesh(PreviewMeshes.Num());
		if (!PreviewMesh)
		{
			continue;
		}

		ConfigurePreviewMesh(PreviewMesh, SourceMesh);
		PreviewMeshes.Add(PreviewMesh);
		SceneCapture->ShowOnlyComponent(PreviewMesh);
	}

	if (PreviewMeshes.IsEmpty())
	{
		return false;
	}

	FramePreviewMeshes();
	CapturePreview();
	return true;
}

void ARogue10mEquipmentPreviewActor::SetPreviewActive(bool bActive)
{
	if (!SceneCapture)
	{
		return;
	}

	SceneCapture->bCaptureEveryFrame = bActive;
	SceneCapture->bCaptureOnMovement = bActive;
	KeyLight->SetVisibility(bActive);
	FillLight->SetVisibility(bActive);
	RimLight->SetVisibility(bActive);
	for (USkeletalMeshComponent* PreviewMesh : PreviewMeshes)
	{
		if (PreviewMesh)
		{
			PreviewMesh->SetVisibility(bActive, true);
		}
	}
	if (bActive)
	{
		CapturePreview();
	}
}

void ARogue10mEquipmentPreviewActor::ClearPreviewMeshes()
{
	if (SceneCapture)
	{
		SceneCapture->ClearShowOnlyComponents();
	}

	for (USkeletalMeshComponent* PreviewMesh : PreviewMeshes)
	{
		if (PreviewMesh)
		{
			PreviewMesh->DestroyComponent();
		}
	}
	PreviewMeshes.Reset();
}

USkeletalMeshComponent* ARogue10mEquipmentPreviewActor::CreatePreviewMesh(int32 MeshIndex)
{
	USkeletalMeshComponent* PreviewMesh = NewObject<USkeletalMeshComponent>(
		this, *FString::Printf(TEXT("Preview Mesh %d"), MeshIndex), RF_Transient);
	if (!PreviewMesh)
	{
		return nullptr;
	}

	PreviewMesh->SetupAttachment(SceneRoot);
	PreviewMesh->RegisterComponent();
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->SetGenerateOverlapEvents(false);
	PreviewMesh->SetCastShadow(false);
	PreviewMesh->SetReceivesDecals(false);
	PreviewMesh->SetVisibleInSceneCaptureOnly(true);
	PreviewMesh->SetComponentTickEnabled(false);
	return PreviewMesh;
}

void ARogue10mEquipmentPreviewActor::ConfigurePreviewMesh(
	USkeletalMeshComponent* TargetMesh, USkeletalMeshComponent* SourceMesh)
{
	TargetMesh->SetSkeletalMeshAsset(SourceMesh->GetSkeletalMeshAsset());
	TargetMesh->SetRelativeTransform(
		SourceMesh->GetComponentTransform().GetRelativeTransform(SourceCharacter->GetActorTransform()));
	TargetMesh->SetLeaderPoseComponent(SourceMesh, true, false);

	for (int32 MaterialIndex = 0; MaterialIndex < SourceMesh->GetNumMaterials(); ++MaterialIndex)
	{
		TargetMesh->SetMaterial(MaterialIndex, SourceMesh->GetMaterial(MaterialIndex));
	}
	TargetMesh->UpdateBounds();
}

void ARogue10mEquipmentPreviewActor::FramePreviewMeshes()
{
	FBox CombinedBounds(ForceInit);
	for (USkeletalMeshComponent* PreviewMesh : PreviewMeshes)
	{
		if (PreviewMesh)
		{
			PreviewMesh->UpdateBounds();
			CombinedBounds += PreviewMesh->Bounds.GetBox();
		}
	}

	if (!CombinedBounds.IsValid)
	{
		return;
	}

	const FVector LocalCenter = GetActorTransform().InverseTransformPosition(CombinedBounds.GetCenter());
	const FVector Extent = CombinedBounds.GetExtent();
	const float HalfFovRadians = FMath::DegreesToRadians(SceneCapture->FOVAngle * 0.5f);
	const float CameraDistance = FMath::Max(
		180.0f,
		(Extent.Z / FMath::Max(0.1f, FMath::Tan(HalfFovRadians))) * PreviewFrameMargin);
	const FVector CameraLocation(CameraDistance, LocalCenter.Y, LocalCenter.Z);
	SceneCapture->SetRelativeLocation(CameraLocation);
	SceneCapture->SetRelativeRotation((LocalCenter - CameraLocation).Rotation());
}

void ARogue10mEquipmentPreviewActor::CapturePreview()
{
	if (SceneCapture && SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
	}
}
