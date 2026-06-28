// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mHUD.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Canvas.h"
#include "Engine/EngineTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mVitalsComponent.h"

ARogue10mHUD::ARogue10mHUD()
{
	CharacterPreviewYaw = 180.0f;
}

void ARogue10mHUD::DrawHUD()
{
	Super::DrawHUD();

	bHasHoveredItem = false;
	HoveredItemSlotIndex = INDEX_NONE;
	EquipmentSlotHitAreas.Reset();
	ItemSlotHitAreas.Reset();

	UpdateWindowDrag();
	DrawRunTimer();
	DrawVitals();
	DrawRunResult();
	DrawInventory();
	DrawItemWindow();
	DrawHoveredItemTooltip();
	DrawDraggedItem();
}

void ARogue10mHUD::ToggleInventory()
{
	SetInventoryVisible(!bInventoryVisible);
}

void ARogue10mHUD::ToggleItemWindow()
{
	SetItemWindowVisible(!bItemWindowVisible);
}

void ARogue10mHUD::SetInventoryVisible(bool bNewInventoryVisible)
{
	bInventoryVisible = bNewInventoryVisible;
	UpdateInventoryCursor();
}

void ARogue10mHUD::SetItemWindowVisible(bool bNewItemWindowVisible)
{
	bItemWindowVisible = bNewItemWindowVisible;
	UpdateInventoryCursor();
}

void ARogue10mHUD::DrawRunTimer()
{
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!Canvas || !RogueGameState)
	{
		return;
	}

	const FString TimerText = FString::Printf(TEXT("Run Timer  %s"), *RogueGameState->GetRemainingTimeText().ToString());
	DrawText(TimerText, TimerColor, 32.0f, 32.0f, nullptr, TimerScale, false);

	const float ProgressWidth = 360.0f;
	const float ProgressHeight = 8.0f;
	const float ProgressX = 34.0f;
	const float ProgressY = 78.0f;
	const float ProgressAlpha = RogueGameState->GetRunProgressAlpha();

	DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.75f), ProgressX, ProgressY, ProgressWidth, ProgressHeight);
	DrawRect(TimerColor, ProgressX, ProgressY, ProgressWidth * ProgressAlpha, ProgressHeight);
}

void ARogue10mHUD::DrawRunResult()
{
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!Canvas || !RogueGameState || RogueGameState->IsRunActive())
	{
		return;
	}

	FString ResultText;
	FLinearColor ResultColor = TimerColor;

	switch (RogueGameState->GetRunPhase())
	{
	case ERogue10mRunPhase::Victory:
		ResultText = TEXT("RUN CLEARED");
		ResultColor = FLinearColor(0.2f, 1.0f, 0.35f, 1.0f);
		break;
	case ERogue10mRunPhase::Defeat:
		ResultText = TEXT("GAME OVER");
		ResultColor = WarningColor;
		break;
	default:
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f - 150.0f;
	const float CenterY = Canvas->SizeY * 0.5f - 40.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), CenterX - 48.0f, CenterY - 28.0f, 396.0f, 116.0f);
	DrawText(ResultText, ResultColor, CenterX, CenterY, nullptr, ResultScale, false);
	DrawText(TEXT("The character died when the run timer expired."), TimerColor, CenterX - 20.0f, CenterY + 52.0f, nullptr, 1.1f, false);
}

void ARogue10mHUD::DrawVitals()
{
	if (!Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!VitalsComponent)
	{
		return;
	}

	const FVector2D BarSize(260.0f, 20.0f);
	const int32 VisibleBarCount = VitalsComponent->ShouldShowMana() ? 3 : 2;
	const FVector2D StartPosition(32.0f, Canvas->SizeY - (VisibleBarCount * 34.0f + 16.0f));
	DrawVitalBar(TEXT("HP"), VitalsComponent->GetHealth(), StartPosition, BarSize, FLinearColor(0.92f, 0.12f, 0.12f, 1.0f));

	float NextBarOffsetY = 34.0f;
	if (VitalsComponent->ShouldShowMana())
	{
		DrawVitalBar(TEXT("MP"), VitalsComponent->GetMana(), StartPosition + FVector2D(0.0f, NextBarOffsetY), BarSize, FLinearColor(0.15f, 0.42f, 1.0f, 1.0f));
		NextBarOffsetY += 34.0f;
	}

	DrawVitalBar(TEXT("SP"), VitalsComponent->GetStamina(), StartPosition + FVector2D(0.0f, NextBarOffsetY), BarSize, FLinearColor(0.16f, 0.86f, 0.32f, 1.0f));
}

void ARogue10mHUD::DrawVitalBar(const FString& Label, const FRogue10mVitalValue& Vital, const FVector2D& Position, const FVector2D& Size, const FLinearColor& FillColor)
{
	const float FillWidth = Size.X * Vital.GetNormalized();
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.58f), Position.X - 4.0f, Position.Y - 4.0f, Size.X + 8.0f, Size.Y + 8.0f);
	DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), Position.X, Position.Y, Size.X, Size.Y);
	DrawRect(FillColor, Position.X, Position.Y, FillWidth, Size.Y);
	DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.18f), Position.X, Position.Y, Size.X, 2.0f);

	DrawText(Label, InventoryTextColor, Position.X + 8.0f, Position.Y + 3.0f, nullptr, 0.82f, false);
	DrawText(FString::Printf(TEXT("%.0f / %.0f"), Vital.Current, Vital.Max), InventoryTextColor, Position.X + Size.X - 86.0f, Position.Y + 3.0f, nullptr, 0.82f, false);
}

void ARogue10mHUD::DrawInventory()
{
	if (!bInventoryVisible || !Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.5f;
	const float PanelHeight = Canvas->SizeY * 0.5f;
	if (InventoryWindowPosition.X < 0.0f || InventoryWindowPosition.Y < 0.0f)
	{
		InventoryWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f, (Canvas->SizeY - PanelHeight) * 0.5f);
	}

	InventoryWindowPosition = ClampWindowPosition(InventoryWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = InventoryWindowPosition.X;
	const float PanelY = InventoryWindowPosition.Y;
	const float SlotSize = FMath::Clamp(PanelHeight * 0.105f, 44.0f, 64.0f);
	const float SlotGap = FMath::Clamp(PanelHeight * 0.022f, 8.0f, 12.0f);

	DrawRect(InventoryPanelColor, PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.1f, 0.95f), PanelX, PanelY, PanelWidth, 56.0f);
	DrawText(TEXT("INVENTORY"), InventoryTextColor, PanelX + 28.0f, PanelY + 22.0f, nullptr, 1.35f, false);
	DrawText(TEXT("I : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 26.0f, nullptr, 0.85f, false);

	DrawCharacterPreview(RogueCharacter, PanelX + PanelWidth * 0.31f, PanelY + 72.0f, PanelWidth * 0.38f, PanelHeight - 96.0f);
	DrawInventorySlots(InventoryComponent->GetLeftEquipmentSlots(), PanelX + 34.0f, PanelY + 74.0f, SlotSize, SlotGap, false);
	DrawInventorySlots(InventoryComponent->GetRightEquipmentSlots(), PanelX + PanelWidth - SlotSize - 34.0f, PanelY + 74.0f, SlotSize, SlotGap, true);
}

void ARogue10mHUD::DrawItemWindow()
{
	if (!bItemWindowVisible || !Canvas)
	{
		return;
	}

	ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.3f;
	const float PanelHeight = Canvas->SizeY * 0.5f;
	if (ItemWindowPosition.X < 0.0f || ItemWindowPosition.Y < 0.0f)
	{
		ItemWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f + 48.0f, (Canvas->SizeY - PanelHeight) * 0.5f + 36.0f);
	}

	ItemWindowPosition = ClampWindowPosition(ItemWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = ItemWindowPosition.X;
	const float PanelY = ItemWindowPosition.Y;
	const float HeaderHeight = 58.0f;

	DrawRect(InventoryPanelColor, PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.1f, 0.96f), PanelX, PanelY, PanelWidth, HeaderHeight);
	DrawText(TEXT("ITEMS"), InventoryTextColor, PanelX + PanelWidth * 0.5f - 36.0f, PanelY + 18.0f, nullptr, 1.35f, false);
	DrawText(TEXT("B : Close"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 112.0f, PanelY + 22.0f, nullptr, 0.85f, false);

	const float TabY = PanelY + HeaderHeight + 10.0f;
	const float TabSize = 38.0f;
	for (int32 Index = 0; Index < 8; ++Index)
	{
		const float TabX = PanelX + 22.0f + Index * (TabSize + 10.0f);
		const FLinearColor TabColor = Index == 1 ? FLinearColor(0.92f, 0.74f, 0.42f, 1.0f) : FLinearColor(0.18f, 0.2f, 0.22f, 1.0f);
		DrawRect(TabColor, TabX, TabY, TabSize, TabSize);
	}

	DrawItemGrid(InventoryComponent, PanelX + 18.0f, PanelY + 112.0f, 48.0f, 6.0f, PanelWidth - 36.0f, PanelHeight - 166.0f);

	const float FooterY = PanelY + PanelHeight - 44.0f;
	DrawRect(FLinearColor(0.03f, 0.04f, 0.05f, 0.96f), PanelX, FooterY, PanelWidth, 44.0f);
	DrawText(FString::Printf(TEXT("Gold  %d"), InventoryComponent->GetGold()), FLinearColor(0.35f, 1.0f, 0.72f, 1.0f), PanelX + PanelWidth * 0.44f, FooterY + 12.0f, nullptr, 1.0f, false);
	DrawText(FString::Printf(TEXT("Crystals  %d"), InventoryComponent->GetCrystals()), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f), PanelX + PanelWidth * 0.72f, FooterY + 12.0f, nullptr, 1.0f, false);
}

void ARogue10mHUD::DrawInventorySlots(const TArray<FRogue10mInventorySlot>& Slots, float X, float Y, float SlotSize, float Gap, bool bRightSide)
{
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FRogue10mInventorySlot& Slot = Slots[Index];
		const float SlotY = Y + Index * (SlotSize + Gap);
		EquipmentSlotHitAreas.Add({ Slot.SlotType, FVector2D(X, SlotY), FVector2D(SlotSize, SlotSize), Slot.bLocked });
		const bool bDragCompatible = bIsDraggingItem && IsItemCompatibleWithSlot(DraggedItem, Slot.SlotType) && !Slot.bLocked;

		DrawRect(Slot.SlotColor, X - 3.0f, SlotY - 3.0f, SlotSize + 6.0f, SlotSize + 6.0f);
		DrawRect(InventorySlotFillColor, X, SlotY, SlotSize, SlotSize);
		if (bDragCompatible)
		{
			DrawRect(FLinearColor(0.2f, 1.0f, 0.55f, 0.22f), X, SlotY, SlotSize, SlotSize);
		}

		const FString SlotName = Slot.DisplayName.ToString();
		DrawText(SlotName, InventoryTextColor, X + 8.0f, SlotY + 10.0f, nullptr, 0.85f, false);

		if (Slot.bLocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), X, SlotY, SlotSize, SlotSize);
			DrawText(TEXT("LOCK"), FLinearColor(0.9f, 0.9f, 0.9f, 1.0f), X + 12.0f, SlotY + 38.0f, nullptr, 0.75f, false);
		}
		else if (Slot.bEquipped)
		{
			DrawRect(FLinearColor(0.1f, 0.9f, 0.95f, 0.16f), X + 8.0f, SlotY + 34.0f, SlotSize - 16.0f, 22.0f);
			DrawText(TEXT("EQUIP"), FLinearColor(0.1f, 0.95f, 1.0f, 1.0f), X + 12.0f, SlotY + 38.0f, nullptr, 0.72f, false);
		}

		const float MarkerX = bRightSide ? X - 18.0f : X + SlotSize + 8.0f;
		DrawText(TEXT("<"), Slot.SlotColor, MarkerX, SlotY + 20.0f, nullptr, 1.0f, false);
	}
}

void ARogue10mHUD::DrawItemGrid(URogue10mInventoryComponent* InventoryComponent, float X, float Y, float SlotSize, float Gap, float MaxWidth, float MaxHeight)
{
	if (!InventoryComponent)
	{
		return;
	}

	const TArray<FRogue10mItemStack>& Items = InventoryComponent->GetItemSlots();
	const int32 Columns = InventoryComponent->GetItemGridColumns();
	const int32 ColumnsByWidth = FMath::Max(1, FMath::FloorToInt((MaxWidth + Gap) / (SlotSize + Gap)));
	const int32 SafeColumns = FMath::Max(1, FMath::Min(Columns, ColumnsByWidth));
	const int32 SafeRows = FMath::Max(1, FMath::FloorToInt((MaxHeight + Gap) / (SlotSize + Gap)));
	const int32 VisibleItemCount = FMath::Min(Items.Num(), SafeColumns * SafeRows);

	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bLeftClickPressed = false;
	bool bLeftClickHeld = false;
	bool bLeftClickReleased = false;
	bool bRightClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bLeftClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
		bLeftClickHeld = OwningPlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
		bLeftClickReleased = OwningPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton);
		bRightClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
	}

	if (bHasMousePosition && bIsDraggingItem)
	{
		DraggedItemMousePosition = MousePosition;
		if (bLeftClickReleased)
		{
			if (const FRogue10mEquipmentSlotHitArea* DropTarget = FindEquipmentSlotHitArea(MousePosition))
			{
				InventoryComponent->TryEquipItemToSlot(DraggedItemSlotIndex, DropTarget->SlotType);
			}
			else
			{
				const float LocalX = MousePosition.X - X;
				const float LocalY = MousePosition.Y - Y;
				const int32 TargetColumn = FMath::FloorToInt(LocalX / (SlotSize + Gap));
				const int32 TargetRow = FMath::FloorToInt(LocalY / (SlotSize + Gap));
				const float TargetSlotX = X + TargetColumn * (SlotSize + Gap);
				const float TargetSlotY = Y + TargetRow * (SlotSize + Gap);
				if (TargetColumn >= 0
					&& TargetColumn < SafeColumns
					&& TargetRow >= 0
					&& TargetRow < SafeRows
					&& IsPointInRect(MousePosition, FVector2D(TargetSlotX, TargetSlotY), FVector2D(SlotSize, SlotSize)))
				{
					const int32 TargetItemSlotIndex = TargetRow * SafeColumns + TargetColumn;
					InventoryComponent->TryMoveItemSlot(DraggedItemSlotIndex, TargetItemSlotIndex);
				}
			}

			bIsDraggingItem = false;
			DraggedItemSlotIndex = INDEX_NONE;
		}
	}
	else if (!bLeftClickHeld)
	{
		bIsDraggingItem = false;
		DraggedItemSlotIndex = INDEX_NONE;
	}

	for (int32 Index = 0; Index < VisibleItemCount; ++Index)
	{
		const int32 Row = Index / SafeColumns;
		const int32 Column = Index % SafeColumns;
		const float SlotX = X + Column * (SlotSize + Gap);
		const float SlotY = Y + Row * (SlotSize + Gap);
		const FRogue10mItemStack& Item = Items[Index];
		const FVector2D SlotPosition(SlotX, SlotY);
		const FVector2D SlotExtent(SlotSize, SlotSize);
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, SlotPosition, SlotExtent);
		ItemSlotHitAreas.Add({ Index, SlotPosition, SlotExtent, Item.bLocked });

		DrawRect(FLinearColor(0.01f, 0.012f, 0.015f, 0.95f), SlotX, SlotY, SlotSize, SlotSize);
		DrawRect(FLinearColor(0.13f, 0.16f, 0.19f, 0.75f), SlotX + 1.0f, SlotY + 1.0f, SlotSize - 2.0f, SlotSize - 2.0f);

		if (bIsDraggingItem && Index != DraggedItemSlotIndex)
		{
			DrawRect(FLinearColor(0.2f, 0.65f, 1.0f, 0.1f), SlotX, SlotY, SlotSize, SlotSize);
		}

		if (!Item.bOccupied)
		{
			continue;
		}

		DrawRect(Item.ItemColor, SlotX + 5.0f, SlotY + 5.0f, SlotSize - 10.0f, SlotSize - 10.0f);
		DrawText(Item.DisplayName.ToString(), InventoryTextColor, SlotX + 6.0f, SlotY + 9.0f, nullptr, 0.58f, false);

		if (bHovered)
		{
			DrawRect(FLinearColor(0.95f, 0.82f, 0.35f, 0.28f), SlotX, SlotY, SlotSize, SlotSize);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY - 2.0f, SlotSize + 4.0f, 2.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY + SlotSize, SlotSize + 4.0f, 2.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY - 2.0f, 2.0f, SlotSize + 4.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX + SlotSize, SlotY - 2.0f, 2.0f, SlotSize + 4.0f);

			bHasHoveredItem = true;
			HoveredItem = Item;
			HoveredItemSlotIndex = Index;
			HoveredItemMousePosition = MousePosition;

			if (bRightClickPressed)
			{
				InventoryComponent->TryEquipItemFromSlot(Index);
			}

			if (bLeftClickPressed && !Item.bLocked)
			{
				bIsDraggingItem = true;
				DraggedItem = Item;
				DraggedItemSlotIndex = Index;
				DraggedItemMousePosition = MousePosition;
				bHasHoveredItem = false;
			}
		}

		if (Item.Quantity > 1)
		{
			DrawText(FString::FromInt(Item.Quantity), InventoryTextColor, SlotX + SlotSize - 18.0f, SlotY + 4.0f, nullptr, 0.82f, false);
		}

		if (Item.bLocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), SlotX, SlotY, SlotSize, SlotSize);
			DrawText(TEXT("LOCK"), FLinearColor(0.95f, 0.95f, 0.95f, 1.0f), SlotX + 7.0f, SlotY + SlotSize - 20.0f, nullptr, 0.65f, false);
		}
	}
}

void ARogue10mHUD::DrawHoveredItemTooltip()
{
	if (!Canvas || bIsDraggingItem || !bHasHoveredItem || !HoveredItem.bOccupied)
	{
		return;
	}

	const float TooltipWidth = 300.0f;
	const float TooltipHeight = HoveredItem.Category == ERogue10mItemCategory::Equipment ? 170.0f : 145.0f;
	const FVector2D TooltipSize(TooltipWidth, TooltipHeight);
	FVector2D TooltipPosition = HoveredItemMousePosition + FVector2D(18.0f, 18.0f);
	TooltipPosition = ClampWindowPosition(TooltipPosition, TooltipSize);

	DrawRect(FLinearColor(0.015f, 0.018f, 0.022f, 0.97f), TooltipPosition.X, TooltipPosition.Y, TooltipWidth, TooltipHeight);
	DrawRect(HoveredItem.ItemColor, TooltipPosition.X, TooltipPosition.Y, TooltipWidth, 4.0f);

	const float TextX = TooltipPosition.X + 14.0f;
	float TextY = TooltipPosition.Y + 16.0f;
	DrawText(HoveredItem.DisplayName.ToString(), InventoryTextColor, TextX, TextY, nullptr, 1.05f, false);

	TextY += 28.0f;
	DrawText(FString::Printf(TEXT("%s  |  Qty %d"), *GetItemCategoryText(HoveredItem.Category), HoveredItem.Quantity), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.78f, false);

	if (HoveredItem.Category == ERogue10mItemCategory::Equipment)
	{
		TextY += 22.0f;
		DrawText(FString::Printf(TEXT("Slot : %s"), *GetEquipmentSlotText(HoveredItem.EquipSlotType)), FLinearColor(0.85f, 0.76f, 0.48f, 1.0f), TextX, TextY, nullptr, 0.8f, false);
	}

	TextY += 28.0f;
	DrawText(HoveredItem.Description.ToString(), FLinearColor(0.82f, 0.86f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.72f, false);

	TextY += 38.0f;
	FString ActionText = TEXT("Right-click : Equip");
	FLinearColor ActionColor = FLinearColor(0.35f, 1.0f, 0.72f, 1.0f);
	if (HoveredItem.bLocked)
	{
		ActionText = TEXT("Locked item");
		ActionColor = WarningColor;
	}
	else if (HoveredItem.Category != ERogue10mItemCategory::Equipment)
	{
		ActionText = TEXT("Not equipable");
		ActionColor = FLinearColor(0.68f, 0.72f, 0.78f, 1.0f);
	}

	DrawText(ActionText, ActionColor, TextX, TextY, nullptr, 0.82f, false);
}

void ARogue10mHUD::DrawDraggedItem()
{
	if (!Canvas || !bIsDraggingItem || !DraggedItem.bOccupied)
	{
		return;
	}

	const float DragSlotSize = 58.0f;
	const FVector2D DrawPosition = DraggedItemMousePosition - FVector2D(DragSlotSize * 0.5f, DragSlotSize * 0.5f);
	const FRogue10mEquipmentSlotHitArea* DropTarget = FindEquipmentSlotHitArea(DraggedItemMousePosition);
	const FRogue10mItemSlotHitArea* ItemDropTarget = FindItemSlotHitArea(DraggedItemMousePosition);
	const bool bCanEquipDrop = DropTarget && !DropTarget->bLocked && IsItemCompatibleWithSlot(DraggedItem, DropTarget->SlotType);
	const bool bCanMoveDrop = ItemDropTarget && !ItemDropTarget->bLocked && ItemDropTarget->SlotIndex != DraggedItemSlotIndex;
	const bool bCanDrop = bCanEquipDrop || bCanMoveDrop;
	const FLinearColor BorderColor = bCanDrop ? FLinearColor(0.2f, 1.0f, 0.55f, 0.95f) : FLinearColor(1.0f, 0.35f, 0.25f, 0.85f);

	DrawRect(BorderColor, DrawPosition.X - 3.0f, DrawPosition.Y - 3.0f, DragSlotSize + 6.0f, DragSlotSize + 6.0f);
	DrawRect(FLinearColor(0.02f, 0.025f, 0.03f, 0.92f), DrawPosition.X, DrawPosition.Y, DragSlotSize, DragSlotSize);
	DrawRect(DraggedItem.ItemColor, DrawPosition.X + 6.0f, DrawPosition.Y + 6.0f, DragSlotSize - 12.0f, DragSlotSize - 12.0f);
	DrawText(DraggedItem.DisplayName.ToString(), InventoryTextColor, DrawPosition.X + 7.0f, DrawPosition.Y + 11.0f, nullptr, 0.58f, false);

	const FString DropText = bCanEquipDrop ? TEXT("Release : Equip") : (bCanMoveDrop ? TEXT("Release : Move") : TEXT("Drop on slot"));
	DrawText(DropText, BorderColor, DrawPosition.X + DragSlotSize + 10.0f, DrawPosition.Y + 18.0f, nullptr, 0.8f, false);
}

void ARogue10mHUD::DrawCharacterPreview(const ARogue10mCharacter* RogueCharacter, float X, float Y, float Width, float Height)
{
	CharacterPreviewPosition = FVector2D(X, Y);
	CharacterPreviewSize = FVector2D(Width, Height);

	const int32 RenderTargetWidth = FMath::Max(256, FMath::RoundToInt(Width * 2.0f));
	const int32 RenderTargetHeight = FMath::Max(384, FMath::RoundToInt(Height * 2.0f));
	if (!EnsureCharacterPreview(RogueCharacter, RenderTargetWidth, RenderTargetHeight))
	{
		DrawText(TEXT("Character preview unavailable"), FLinearColor(0.75f, 0.82f, 0.9f, 1.0f), X + 20.0f, Y + Height * 0.5f, nullptr, 0.9f, false);
		return;
	}

	UpdateCharacterPreview(RogueCharacter);
	DrawTexture(CharacterPreviewRenderTarget, X, Y, Width, Height, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::White, BLEND_Translucent);
}

bool ARogue10mHUD::EnsureCharacterPreview(const ARogue10mCharacter* RogueCharacter, int32 RenderTargetWidth, int32 RenderTargetHeight)
{
	if (!RogueCharacter || !GetWorld())
	{
		return false;
	}

	if (!CharacterPreviewActor)
	{
		CharacterPreviewActor = GetWorld()->SpawnActor<AActor>();
		if (!CharacterPreviewActor)
		{
			return false;
		}

		CharacterPreviewActor->SetActorHiddenInGame(false);
		CharacterPreviewActor->SetActorEnableCollision(false);
	}

	if (!CharacterPreviewMesh)
	{
		CharacterPreviewMesh = NewObject<USkeletalMeshComponent>(CharacterPreviewActor);
		CharacterPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterPreviewMesh->SetCastShadow(false);
		CharacterPreviewMesh->SetVisibility(true);
		CharacterPreviewMesh->SetHiddenInGame(false);
		CharacterPreviewMesh->RegisterComponentWithWorld(GetWorld());
		CharacterPreviewActor->SetRootComponent(CharacterPreviewMesh);
	}

	if (!CharacterPreviewCapture)
	{
		CharacterPreviewCapture = NewObject<USceneCaptureComponent2D>(CharacterPreviewActor);
		CharacterPreviewCapture->ProjectionType = ECameraProjectionMode::Perspective;
		CharacterPreviewCapture->FOVAngle = 30.0f;
		CharacterPreviewCapture->bCaptureEveryFrame = false;
		CharacterPreviewCapture->bCaptureOnMovement = false;
		CharacterPreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		CharacterPreviewCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		CharacterPreviewCapture->ShowOnlyComponents.Add(CharacterPreviewMesh);
		CharacterPreviewCapture->RegisterComponentWithWorld(GetWorld());
	}

	if (!CharacterPreviewRenderTarget)
	{
		CharacterPreviewRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		CharacterPreviewRenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (CharacterPreviewRenderTarget->SizeX != RenderTargetWidth || CharacterPreviewRenderTarget->SizeY != RenderTargetHeight)
	{
		CharacterPreviewRenderTarget->InitAutoFormat(RenderTargetWidth, RenderTargetHeight);
		CharacterPreviewCapture->TextureTarget = CharacterPreviewRenderTarget;
	}

	return CharacterPreviewRenderTarget && CharacterPreviewCapture && CharacterPreviewMesh;
}

void ARogue10mHUD::UpdateCharacterPreview(const ARogue10mCharacter* RogueCharacter)
{
	if (!RogueCharacter || !CharacterPreviewMesh || !CharacterPreviewCapture)
	{
		return;
	}

	const USkeletalMeshComponent* SourceMesh = RogueCharacter->GetMesh();
	if (!SourceMesh || !SourceMesh->GetSkeletalMeshAsset())
	{
		SourceMesh = RogueCharacter->GetFirstPersonMesh();
	}

	if (!SourceMesh || !SourceMesh->GetSkeletalMeshAsset())
	{
		return;
	}

	if (CharacterPreviewMesh->GetSkeletalMeshAsset() != SourceMesh->GetSkeletalMeshAsset())
	{
		CharacterPreviewMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
		CharacterPreviewMesh->SetLeaderPoseComponent(const_cast<USkeletalMeshComponent*>(SourceMesh));
	}

	const int32 MaterialCount = SourceMesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		CharacterPreviewMesh->SetMaterial(MaterialIndex, SourceMesh->GetMaterial(MaterialIndex));
	}

	const FVector PreviewLocation = RogueCharacter->GetActorLocation() + FVector(0.0f, 0.0f, -5000.0f);
	CharacterPreviewActor->SetActorLocation(PreviewLocation);
	CharacterPreviewMesh->SetWorldLocation(PreviewLocation);
	CharacterPreviewMesh->SetWorldRotation(FRotator(0.0f, CharacterPreviewYaw, 0.0f));

	const FVector FocusLocation = PreviewLocation + FVector(0.0f, 0.0f, 65.0f);
	const FVector CameraLocation = FocusLocation + FVector(-230.0f, 0.0f, 18.0f);
	CharacterPreviewCapture->SetWorldLocation(CameraLocation);
	CharacterPreviewCapture->SetWorldRotation((FocusLocation - CameraLocation).Rotation());
	CharacterPreviewCapture->TextureTarget = CharacterPreviewRenderTarget;
	CharacterPreviewCapture->CaptureScene();
}

void ARogue10mHUD::UpdateWindowDrag()
{
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!Canvas || !OwningPlayerController || !IsAnyInventoryWindowVisible())
	{
		DraggedWindow = ERogue10mDraggedWindow::None;
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!OwningPlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	const FVector2D MousePosition(MouseX, MouseY);
	const bool bPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	const bool bHeld = OwningPlayerController->IsInputKeyDown(EKeys::LeftMouseButton);

	const FVector2D InventorySize(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
	const FVector2D ItemSize(Canvas->SizeX * 0.3f, Canvas->SizeY * 0.5f);
	const bool bMouseOverItemWindow = bItemWindowVisible && IsPointInRect(MousePosition, ItemWindowPosition, ItemSize);

	if (bPressed)
	{
		if (bItemWindowVisible && IsPointInRect(MousePosition, ItemWindowPosition, FVector2D(ItemSize.X, 58.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::Items;
			DragOffset = MousePosition - ItemWindowPosition;
		}
		else if (bInventoryVisible && IsPointInRect(MousePosition, InventoryWindowPosition, FVector2D(InventorySize.X, 56.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::Equipment;
			DragOffset = MousePosition - InventoryWindowPosition;
		}
		else if (bInventoryVisible && !bMouseOverItemWindow && IsPointInRect(MousePosition, CharacterPreviewPosition, CharacterPreviewSize))
		{
			DraggedWindow = ERogue10mDraggedWindow::CharacterPreview;
			LastCharacterPreviewDragX = MousePosition.X;
		}
	}

	if (!bHeld)
	{
		DraggedWindow = ERogue10mDraggedWindow::None;
		return;
	}

	switch (DraggedWindow)
	{
	case ERogue10mDraggedWindow::Equipment:
		InventoryWindowPosition = ClampWindowPosition(MousePosition - DragOffset, InventorySize);
		break;
	case ERogue10mDraggedWindow::Items:
		ItemWindowPosition = ClampWindowPosition(MousePosition - DragOffset, ItemSize);
		break;
	case ERogue10mDraggedWindow::CharacterPreview:
		CharacterPreviewYaw = FMath::Fmod(CharacterPreviewYaw + (MousePosition.X - LastCharacterPreviewDragX) * 0.45f, 360.0f);
		LastCharacterPreviewDragX = MousePosition.X;
		break;
	default:
		break;
	}
}

void ARogue10mHUD::UpdateInventoryCursor() const
{
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		OwningPlayerController->SetShowMouseCursor(IsAnyInventoryWindowVisible());
	}
}

FString ARogue10mHUD::GetItemCategoryText(ERogue10mItemCategory Category) const
{
	switch (Category)
	{
	case ERogue10mItemCategory::Equipment:
		return TEXT("Equipment");
	case ERogue10mItemCategory::Consumable:
		return TEXT("Consumable");
	case ERogue10mItemCategory::Material:
		return TEXT("Material");
	case ERogue10mItemCategory::Currency:
		return TEXT("Currency");
	case ERogue10mItemCategory::Quest:
		return TEXT("Quest");
	default:
		return TEXT("Unknown");
	}
}

FString ARogue10mHUD::GetEquipmentSlotText(ERogue10mInventorySlotType SlotType) const
{
	switch (SlotType)
	{
	case ERogue10mInventorySlotType::Head:
		return TEXT("Head");
	case ERogue10mInventorySlotType::Chest:
		return TEXT("Chest");
	case ERogue10mInventorySlotType::Legs:
		return TEXT("Legs");
	case ERogue10mInventorySlotType::Hands:
		return TEXT("Hands");
	case ERogue10mInventorySlotType::Weapon:
		return TEXT("Weapon");
	case ERogue10mInventorySlotType::Charm:
		return TEXT("Charm");
	case ERogue10mInventorySlotType::Ring:
		return TEXT("Ring");
	case ERogue10mInventorySlotType::Relic:
		return TEXT("Relic");
	case ERogue10mInventorySlotType::Consumable:
		return TEXT("Consumable");
	case ERogue10mInventorySlotType::Material:
		return TEXT("Material");
	default:
		return TEXT("Unknown");
	}
}

const FRogue10mEquipmentSlotHitArea* ARogue10mHUD::FindEquipmentSlotHitArea(const FVector2D& MousePosition) const
{
	for (const FRogue10mEquipmentSlotHitArea& HitArea : EquipmentSlotHitAreas)
	{
		if (IsPointInRect(MousePosition, HitArea.Position, HitArea.Size))
		{
			return &HitArea;
		}
	}

	return nullptr;
}

const FRogue10mItemSlotHitArea* ARogue10mHUD::FindItemSlotHitArea(const FVector2D& MousePosition) const
{
	for (const FRogue10mItemSlotHitArea& HitArea : ItemSlotHitAreas)
	{
		if (IsPointInRect(MousePosition, HitArea.Position, HitArea.Size))
		{
			return &HitArea;
		}
	}

	return nullptr;
}

bool ARogue10mHUD::IsItemCompatibleWithSlot(const FRogue10mItemStack& Item, ERogue10mInventorySlotType SlotType) const
{
	return Item.bOccupied
		&& !Item.bLocked
		&& Item.Category == ERogue10mItemCategory::Equipment
		&& Item.EquipSlotType == SlotType;
}

bool ARogue10mHUD::IsPointInRect(const FVector2D& Point, const FVector2D& RectPosition, const FVector2D& RectSize) const
{
	return Point.X >= RectPosition.X
		&& Point.Y >= RectPosition.Y
		&& Point.X <= RectPosition.X + RectSize.X
		&& Point.Y <= RectPosition.Y + RectSize.Y;
}

FVector2D ARogue10mHUD::ClampWindowPosition(const FVector2D& Position, const FVector2D& Size) const
{
	if (!Canvas)
	{
		return Position;
	}

	const float MaxX = FMath::Max(0.0f, Canvas->SizeX - Size.X);
	const float MaxY = FMath::Max(0.0f, Canvas->SizeY - Size.Y);
	return FVector2D(FMath::Clamp(Position.X, 0.0f, MaxX), FMath::Clamp(Position.Y, 0.0f, MaxY));
}
