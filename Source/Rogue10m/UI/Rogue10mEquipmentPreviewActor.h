// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rogue10mEquipmentPreviewActor.generated.h"

class ACharacter;
class UPointLightComponent;
class USceneCaptureComponent2D;
class USceneComponent;
class USkeletalMeshComponent;
class UTextureRenderTarget2D;

/** 장비창에 현재 플레이어 외형을 렌더링하는 transient 전용 액터입니다. */
UCLASS(NotBlueprintable, NotPlaceable, Transient)
class ROGUE10M_API ARogue10mEquipmentPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ARogue10mEquipmentPreviewActor();

	bool InitializePreview(ACharacter* InSourceCharacter, FIntPoint InResolution);
	bool RefreshFromCharacter();
	void SetPreviewActive(bool bActive);

	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

private:
	void ClearPreviewMeshes();
	USkeletalMeshComponent* CreatePreviewMesh(int32 MeshIndex);
	void ConfigurePreviewMesh(USkeletalMeshComponent* TargetMesh, USkeletalMeshComponent* SourceMesh);
	void FramePreviewMeshes();
	void CapturePreview();

	UPROPERTY(VisibleAnywhere, Category="Rogue10m|Equipment Preview")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="Rogue10m|Equipment Preview")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(VisibleAnywhere, Category="Rogue10m|Equipment Preview")
	TObjectPtr<UPointLightComponent> KeyLight;

	UPROPERTY(VisibleAnywhere, Category="Rogue10m|Equipment Preview")
	TObjectPtr<UPointLightComponent> FillLight;

	UPROPERTY(VisibleAnywhere, Category="Rogue10m|Equipment Preview")
	TObjectPtr<UPointLightComponent> RimLight;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USkeletalMeshComponent>> PreviewMeshes;

	TWeakObjectPtr<ACharacter> SourceCharacter;
};
