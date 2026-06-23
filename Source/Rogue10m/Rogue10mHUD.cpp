// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mHUD.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"

void ARogue10mHUD::DrawHUD()
{
	Super::DrawHUD();

	UpdateWindowDrag();
	DrawRunTimer();
	DrawRunResult();
	DrawInventory();
	DrawItemWindow();
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
		ResultText = TEXT("RUN FAILED");
		ResultColor = WarningColor;
		break;
	default:
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f - 150.0f;
	const float CenterY = Canvas->SizeY * 0.5f - 40.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), CenterX - 48.0f, CenterY - 28.0f, 396.0f, 116.0f);
	DrawText(ResultText, ResultColor, CenterX, CenterY, nullptr, ResultScale, false);
	DrawText(TEXT("Prototype run ended after 30 seconds."), TimerColor, CenterX - 20.0f, CenterY + 52.0f, nullptr, 1.1f, false);
}

void ARogue10mHUD::DrawInventory()
{
	if (!bInventoryVisible || !Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = FMath::Min(Canvas->SizeX * 0.86f, 920.0f);
	const float PanelHeight = FMath::Min(Canvas->SizeY * 0.84f, 620.0f);
	if (InventoryWindowPosition.X < 0.0f || InventoryWindowPosition.Y < 0.0f)
	{
		InventoryWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f, (Canvas->SizeY - PanelHeight) * 0.5f);
	}

	InventoryWindowPosition = ClampWindowPosition(InventoryWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = InventoryWindowPosition.X;
	const float PanelY = InventoryWindowPosition.Y;
	const float SlotSize = 72.0f;
	const float SlotGap = 14.0f;

	DrawRect(InventoryPanelColor, PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.1f, 0.95f), PanelX, PanelY, PanelWidth, 56.0f);
	DrawText(TEXT("INVENTORY"), InventoryTextColor, PanelX + 28.0f, PanelY + 22.0f, nullptr, 1.35f, false);
	DrawText(TEXT("I : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 26.0f, nullptr, 0.85f, false);

	DrawCharacterPreview(PanelX + PanelWidth * 0.33f, PanelY + 82.0f, PanelWidth * 0.34f, PanelHeight - 120.0f);
	DrawInventorySlots(InventoryComponent->GetLeftEquipmentSlots(), PanelX + 34.0f, PanelY + 74.0f, SlotSize, SlotGap, false);
	DrawInventorySlots(InventoryComponent->GetRightEquipmentSlots(), PanelX + PanelWidth - SlotSize - 34.0f, PanelY + 74.0f, SlotSize, SlotGap, true);
}

void ARogue10mHUD::DrawItemWindow()
{
	if (!bItemWindowVisible || !Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = FMath::Min(Canvas->SizeX * 0.9f, 820.0f);
	const float PanelHeight = FMath::Min(Canvas->SizeY * 0.88f, 720.0f);
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
	DrawText(TEXT("B : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 22.0f, nullptr, 0.85f, false);

	const float TabY = PanelY + HeaderHeight + 10.0f;
	const float TabSize = 38.0f;
	for (int32 Index = 0; Index < 8; ++Index)
	{
		const float TabX = PanelX + 22.0f + Index * (TabSize + 10.0f);
		const FLinearColor TabColor = Index == 1 ? FLinearColor(0.92f, 0.74f, 0.42f, 1.0f) : FLinearColor(0.18f, 0.2f, 0.22f, 1.0f);
		DrawRect(TabColor, TabX, TabY, TabSize, TabSize);
	}

	DrawItemGrid(InventoryComponent->GetItemSlots(), InventoryComponent->GetItemGridColumns(), PanelX + 22.0f, PanelY + 118.0f, 54.0f, 6.0f);

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

		DrawRect(Slot.SlotColor, X - 3.0f, SlotY - 3.0f, SlotSize + 6.0f, SlotSize + 6.0f);
		DrawRect(InventorySlotFillColor, X, SlotY, SlotSize, SlotSize);

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

void ARogue10mHUD::DrawItemGrid(const TArray<FRogue10mItemStack>& Items, int32 Columns, float X, float Y, float SlotSize, float Gap)
{
	const int32 SafeColumns = FMath::Max(1, Columns);
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		const int32 Row = Index / SafeColumns;
		const int32 Column = Index % SafeColumns;
		const float SlotX = X + Column * (SlotSize + Gap);
		const float SlotY = Y + Row * (SlotSize + Gap);
		const FRogue10mItemStack& Item = Items[Index];

		DrawRect(FLinearColor(0.01f, 0.012f, 0.015f, 0.95f), SlotX, SlotY, SlotSize, SlotSize);
		DrawRect(FLinearColor(0.13f, 0.16f, 0.19f, 0.75f), SlotX + 1.0f, SlotY + 1.0f, SlotSize - 2.0f, SlotSize - 2.0f);

		if (!Item.bOccupied)
		{
			continue;
		}

		DrawRect(Item.ItemColor, SlotX + 5.0f, SlotY + 5.0f, SlotSize - 10.0f, SlotSize - 10.0f);
		DrawText(Item.DisplayName.ToString(), InventoryTextColor, SlotX + 6.0f, SlotY + 9.0f, nullptr, 0.58f, false);

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

void ARogue10mHUD::DrawCharacterPreview(float X, float Y, float Width, float Height)
{
	DrawRect(FLinearColor(0.02f, 0.035f, 0.055f, 0.8f), X, Y, Width, Height);
	DrawText(TEXT("UNARMED"), FLinearColor(0.8f, 0.85f, 0.9f, 1.0f), X + Width * 0.5f - 48.0f, Y + 18.0f, nullptr, 1.0f, false);

	const float CenterX = X + Width * 0.5f;
	const float HeadY = Y + Height * 0.23f;
	const float BodyY = Y + Height * 0.38f;
	const float LegY = Y + Height * 0.66f;

	DrawRect(FLinearColor(0.92f, 0.88f, 0.82f, 1.0f), CenterX - 18.0f, HeadY - 20.0f, 36.0f, 40.0f);
	DrawRect(FLinearColor(0.015f, 0.015f, 0.018f, 1.0f), CenterX - 46.0f, BodyY - 28.0f, 92.0f, 130.0f);
	DrawLine(CenterX - 46.0f, BodyY, CenterX - 98.0f, BodyY + 92.0f, FLinearColor(0.015f, 0.015f, 0.018f, 1.0f), 8.0f);
	DrawLine(CenterX + 46.0f, BodyY, CenterX + 98.0f, BodyY + 92.0f, FLinearColor(0.015f, 0.015f, 0.018f, 1.0f), 8.0f);
	DrawLine(CenterX - 24.0f, LegY, CenterX - 48.0f, Y + Height - 34.0f, FLinearColor(0.92f, 0.88f, 0.82f, 1.0f), 12.0f);
	DrawLine(CenterX + 24.0f, LegY, CenterX + 48.0f, Y + Height - 34.0f, FLinearColor(0.92f, 0.88f, 0.82f, 1.0f), 12.0f);
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

	const FVector2D InventorySize(FMath::Min(Canvas->SizeX * 0.86f, 920.0f), FMath::Min(Canvas->SizeY * 0.84f, 620.0f));
	const FVector2D ItemSize(FMath::Min(Canvas->SizeX * 0.9f, 820.0f), FMath::Min(Canvas->SizeY * 0.88f, 720.0f));

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
