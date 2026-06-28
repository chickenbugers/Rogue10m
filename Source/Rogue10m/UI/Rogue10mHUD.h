// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mHUD.generated.h"

enum class ERogue10mDraggedWindow : uint8
{
	None,
	Equipment,
	Items,
	CharacterPreview
};

class AActor;
class ARogue10mCharacter;
class USceneCaptureComponent2D;
class USkeletalMeshComponent;
class UTextureRenderTarget2D;

struct FRogue10mEquipmentSlotHitArea
{
	ERogue10mInventorySlotType SlotType = ERogue10mInventorySlotType::Material;
	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Size = FVector2D::ZeroVector;
	bool bLocked = false;
};

struct FRogue10mItemSlotHitArea
{
	int32 SlotIndex = INDEX_NONE;
	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Size = FVector2D::ZeroVector;
	bool bLocked = false;
};

/**
 * Minimal C++ HUD for prototype run state.
 */
UCLASS()
class ROGUE10M_API ARogue10mHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARogue10mHUD();

	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void ToggleItemWindow();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void SetInventoryVisible(bool bNewInventoryVisible);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void SetItemWindowVisible(bool bNewItemWindowVisible);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsInventoryVisible() const { return bInventoryVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsItemWindowVisible() const { return bItemWindowVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsAnyInventoryWindowVisible() const { return bInventoryVisible || bItemWindowVisible; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor TimerColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor WarningColor = FLinearColor(1.0f, 0.2f, 0.15f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD")
	float TimerScale = 1.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD")
	float ResultScale = 2.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FLinearColor InventoryPanelColor = FLinearColor(0.015f, 0.02f, 0.03f, 0.92f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FLinearColor InventorySlotFillColor = FLinearColor(0.08f, 0.075f, 0.055f, 0.95f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FLinearColor InventoryTextColor = FLinearColor::White;

private:
	void DrawRunTimer();
	void DrawRunResult();
	void DrawVitals();
	void DrawVitalBar(const FString& Label, const struct FRogue10mVitalValue& Vital, const FVector2D& Position, const FVector2D& Size, const FLinearColor& FillColor);
	void DrawInventory();
	void DrawItemWindow();
	void DrawInventorySlots(const TArray<struct FRogue10mInventorySlot>& Slots, float X, float Y, float SlotSize, float Gap, bool bRightSide);
	void DrawItemGrid(class URogue10mInventoryComponent* InventoryComponent, float X, float Y, float SlotSize, float Gap, float MaxWidth, float MaxHeight);
	void DrawHoveredItemTooltip();
	void DrawDraggedItem();
	void DrawCharacterPreview(const ARogue10mCharacter* RogueCharacter, float X, float Y, float Width, float Height);
	void UpdateWindowDrag();
	void UpdateInventoryCursor() const;
	bool EnsureCharacterPreview(const ARogue10mCharacter* RogueCharacter, int32 RenderTargetWidth, int32 RenderTargetHeight);
	void UpdateCharacterPreview(const ARogue10mCharacter* RogueCharacter);
	FString GetItemCategoryText(ERogue10mItemCategory Category) const;
	FString GetEquipmentSlotText(ERogue10mInventorySlotType SlotType) const;
	const FRogue10mEquipmentSlotHitArea* FindEquipmentSlotHitArea(const FVector2D& MousePosition) const;
	const FRogue10mItemSlotHitArea* FindItemSlotHitArea(const FVector2D& MousePosition) const;
	bool IsItemCompatibleWithSlot(const FRogue10mItemStack& Item, ERogue10mInventorySlotType SlotType) const;
	bool IsPointInRect(const FVector2D& Point, const FVector2D& RectPosition, const FVector2D& RectSize) const;
	FVector2D ClampWindowPosition(const FVector2D& Position, const FVector2D& Size) const;

	bool bInventoryVisible = false;
	bool bItemWindowVisible = false;
	ERogue10mDraggedWindow DraggedWindow = ERogue10mDraggedWindow::None;
	FVector2D DragOffset = FVector2D::ZeroVector;
	FVector2D InventoryWindowPosition = FVector2D(-1.0f, -1.0f);
	FVector2D ItemWindowPosition = FVector2D(-1.0f, -1.0f);
	FVector2D CharacterPreviewPosition = FVector2D::ZeroVector;
	FVector2D CharacterPreviewSize = FVector2D::ZeroVector;
	float CharacterPreviewYaw = 180.0f;
	float LastCharacterPreviewDragX = 0.0f;
	bool bHasHoveredItem = false;
	FRogue10mItemStack HoveredItem;
	FVector2D HoveredItemMousePosition = FVector2D::ZeroVector;
	int32 HoveredItemSlotIndex = INDEX_NONE;
	bool bIsDraggingItem = false;
	FRogue10mItemStack DraggedItem;
	FVector2D DraggedItemMousePosition = FVector2D::ZeroVector;
	int32 DraggedItemSlotIndex = INDEX_NONE;
	TArray<FRogue10mEquipmentSlotHitArea> EquipmentSlotHitAreas;
	TArray<FRogue10mItemSlotHitArea> ItemSlotHitAreas;

	UPROPERTY(Transient)
	TObjectPtr<AActor> CharacterPreviewActor;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> CharacterPreviewMesh;

	UPROPERTY(Transient)
	TObjectPtr<USceneCaptureComponent2D> CharacterPreviewCapture;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> CharacterPreviewRenderTarget;
};
