// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacterLobbyWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"
#include "Rogue10mCharacterCustomizationDataAsset.h"
#include "Rogue10mCharacterCustomizationPreviewActor.h"
#include "Rogue10mCharacterProfileSubsystem.h"
#include "Rogue10mPlayerController.h"

namespace
{
	FText IndexedLabel(const TCHAR* Label, int32 Index, bool bOptional)
	{
		return bOptional && Index == INDEX_NONE
			? FText::Format(NSLOCTEXT("Rogue10mCharacterLobby", "OptionalNone", "{0}: 없음"), FText::FromString(Label))
			: FText::Format(NSLOCTEXT("Rogue10mCharacterLobby", "IndexedValue", "{0}: {1}"), FText::FromString(Label), FText::AsNumber(Index + 1));
	}

	FText RaceLabel(ERogue10mCharacterRace Race)
	{
		switch (Race)
		{
		case ERogue10mCharacterRace::Dwarf: return NSLOCTEXT("Rogue10mCharacterLobby", "Dwarf", "드워프");
		case ERogue10mCharacterRace::Orc: return NSLOCTEXT("Rogue10mCharacterLobby", "Orc", "오크");
		default: return NSLOCTEXT("Rogue10mCharacterLobby", "Human", "인간");
		}
	}

	FText GenderLabel(ERogue10mCharacterGender Gender)
	{
		return Gender == ERogue10mCharacterGender::Female
			? NSLOCTEXT("Rogue10mCharacterLobby", "Female", "여성")
			: NSLOCTEXT("Rogue10mCharacterLobby", "Male", "남성");
	}
}

void URogue10mCharacterLobbyWidget::InitializeCharacterLobby(
	ARogue10mPlayerController* InController)
{
	RogueController = InController;
	ProfileSubsystem = InController && InController->GetGameInstance()
		? InController->GetGameInstance()->GetSubsystem<URogue10mCharacterProfileSubsystem>()
		: nullptr;
	Catalog = CustomizationCatalog.LoadSynchronous();
	RefreshAll();
}

void URogue10mCharacterLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	UI_Slot1Button->OnClicked.AddDynamic(this, &ThisClass::HandleSlot1Clicked);
	UI_Slot2Button->OnClicked.AddDynamic(this, &ThisClass::HandleSlot2Clicked);
	UI_Slot3Button->OnClicked.AddDynamic(this, &ThisClass::HandleSlot3Clicked);
	UI_NewCharacterButton->OnClicked.AddDynamic(this, &ThisClass::HandleNewCharacterClicked);
	UI_DeleteCharacterButton->OnClicked.AddDynamic(this, &ThisClass::HandleDeleteCharacterClicked);
	UI_EnterGameButton->OnClicked.AddDynamic(this, &ThisClass::HandleEnterGameClicked);
	UI_CreateCharacterButton->OnClicked.AddDynamic(this, &ThisClass::HandleCreateCharacterClicked);
	UI_CancelCreationButton->OnClicked.AddDynamic(this, &ThisClass::HandleCancelCreationClicked);
	UI_RandomizeButton->OnClicked.AddDynamic(this, &ThisClass::HandleRandomizeClicked);
	UI_HumanRaceButton->OnClicked.AddDynamic(this, &ThisClass::HandleHumanRaceClicked);
	UI_DwarfRaceButton->OnClicked.AddDynamic(this, &ThisClass::HandleDwarfRaceClicked);
	UI_OrcRaceButton->OnClicked.AddDynamic(this, &ThisClass::HandleOrcRaceClicked);
	UI_MaleGenderButton->OnClicked.AddDynamic(this, &ThisClass::HandleMaleGenderClicked);
	UI_FemaleGenderButton->OnClicked.AddDynamic(this, &ThisClass::HandleFemaleGenderClicked);
	UI_HeadPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleHeadPreviousClicked);
	UI_HeadNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleHeadNextClicked);
	UI_SkinPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleSkinPreviousClicked);
	UI_SkinNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleSkinNextClicked);
	UI_HairPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleHairPreviousClicked);
	UI_HairNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleHairNextClicked);
	UI_HairColorPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleHairColorPreviousClicked);
	UI_HairColorNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleHairColorNextClicked);
	UI_FacialPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleFacialPreviousClicked);
	UI_FacialNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleFacialNextClicked);
	UI_EyePreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleEyePreviousClicked);
	UI_EyeNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleEyeNextClicked);
	UI_BodyDetailPreviousButton->OnClicked.AddDynamic(this, &ThisClass::HandleBodyDetailPreviousClicked);
	UI_BodyDetailNextButton->OnClicked.AddDynamic(this, &ThisClass::HandleBodyDetailNextClicked);

	if (GetWorld() && Catalog)
	{
		PreviewActor = GetWorld()->SpawnActor<ARogue10mCharacterCustomizationPreviewActor>();
		if (PreviewActor && PreviewActor->InitializePreview(Catalog, FIntPoint(720, 900)))
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(PreviewActor->GetRenderTarget());
			Brush.ImageSize = FVector2D(720.0f, 900.0f);
			Brush.DrawAs = ESlateBrushDrawType::Image;
			UI_CharacterPreviewImage->SetBrush(Brush);
		}
	}
	RefreshAll();
}

void URogue10mCharacterLobbyWidget::NativeDestruct()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
	bDraggingPreview = false;
	Super::NativeDestruct();
}

FReply URogue10mCharacterLobbyWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
		&& UI_CharacterPreviewImage
		&& UI_CharacterPreviewImage->GetCachedGeometry().IsUnderLocation(
			InMouseEvent.GetScreenSpacePosition()))
	{
		bDraggingPreview = true;
		LastPreviewDragPosition = InMouseEvent.GetScreenSpacePosition();
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply URogue10mCharacterLobbyWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDraggingPreview && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDraggingPreview = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply URogue10mCharacterLobbyWidget::NativeOnMouseMove(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDraggingPreview && PreviewActor)
	{
		const FVector2D CurrentPosition = InMouseEvent.GetScreenSpacePosition();
		const float DeltaX = CurrentPosition.X - LastPreviewDragPosition.X;
		LastPreviewDragPosition = CurrentPosition;
		PreviewActor->AddPreviewYaw(DeltaX * PreviewDragYawSensitivity);
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void URogue10mCharacterLobbyWidget::NativeOnMouseCaptureLost(
	const FCaptureLostEvent& CaptureLostEvent)
{
	bDraggingPreview = false;
	Super::NativeOnMouseCaptureLost(CaptureLostEvent);
}

void URogue10mCharacterLobbyWidget::RefreshAll()
{
	RefreshSlotButtons();
	if (!ProfileSubsystem || !Catalog)
	{
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "InitializationFailed", "캐릭터 로비 데이터를 초기화하지 못했습니다."), true);
		return;
	}

	if (ProfileSubsystem->GetProfileCount() == 0 && !bCreationMode)
	{
		BeginCreation();
		return;
	}

	if (!bCreationMode && !SelectedProfileId.IsValid())
	{
		if (const FRogue10mCharacterProfile* Selected = ProfileSubsystem->GetSelectedProfile())
		{
			SelectedProfileId = Selected->ProfileId;
		}
		else if (!ProfileSubsystem->GetProfiles().IsEmpty())
		{
			SelectedProfileId = ProfileSubsystem->GetProfiles()[0].ProfileId;
		}
	}
	RefreshMode();
}

void URogue10mCharacterLobbyWidget::RefreshSlotButtons()
{
	SlotProfileIds.Init(FGuid(), URogue10mCharacterProfileSubsystem::MaximumProfileCount);
	const TArray<FRogue10mCharacterProfile>* Profiles =
		ProfileSubsystem ? &ProfileSubsystem->GetProfiles() : nullptr;
	UTextBlock* Texts[] = { UI_Slot1Text, UI_Slot2Text, UI_Slot3Text };
	UButton* Buttons[] = { UI_Slot1Button, UI_Slot2Button, UI_Slot3Button };
	for (int32 Index = 0; Index < URogue10mCharacterProfileSubsystem::MaximumProfileCount; ++Index)
	{
		if (Profiles && Profiles->IsValidIndex(Index))
		{
			const FRogue10mCharacterProfile& Profile = (*Profiles)[Index];
			SlotProfileIds[Index] = Profile.ProfileId;
			Texts[Index]->SetText(FText::Format(
				NSLOCTEXT("Rogue10mCharacterLobby", "OccupiedSlot", "{0}\n{1} · {2}"),
				FText::FromString(Profile.CharacterName),
				RaceLabel(Profile.Appearance.Race),
				GenderLabel(Profile.Appearance.Gender)));
		}
		else
		{
			Texts[Index]->SetText(FText::Format(
				NSLOCTEXT("Rogue10mCharacterLobby", "EmptySlot", "슬롯 {0}\n새 캐릭터"),
				FText::AsNumber(Index + 1)));
		}
		Buttons[Index]->SetIsEnabled(!bCreationMode);
	}
}

void URogue10mCharacterLobbyWidget::RefreshMode()
{
	UI_CreationPanel->SetVisibility(
		bCreationMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UI_EnterGameButton->SetVisibility(
		!bCreationMode && SelectedProfileId.IsValid()
			? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UI_DeleteCharacterButton->SetVisibility(
		!bCreationMode && SelectedProfileId.IsValid()
			? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UI_NewCharacterButton->SetVisibility(
		!bCreationMode && ProfileSubsystem
		&& ProfileSubsystem->GetProfileCount() < URogue10mCharacterProfileSubsystem::MaximumProfileCount
			? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (bCreationMode)
	{
		RefreshDraft();
		return;
	}

	const FRogue10mCharacterProfile* Profile =
		ProfileSubsystem ? ProfileSubsystem->FindProfile(SelectedProfileId) : nullptr;
	if (Profile)
	{
		UI_SelectedCharacterInfoText->SetText(FText::Format(
			NSLOCTEXT("Rogue10mCharacterLobby", "SelectedCharacterInfo", "{0}\n{1} · {2}"),
			FText::FromString(Profile->CharacterName),
			RaceLabel(Profile->Appearance.Race),
			GenderLabel(Profile->Appearance.Gender)));
		RefreshPreview(Profile->Appearance);
	}
	else
	{
		UI_SelectedCharacterInfoText->SetText(
			NSLOCTEXT("Rogue10mCharacterLobby", "SelectCharacter", "접속할 캐릭터를 선택하세요."));
	}
}

void URogue10mCharacterLobbyWidget::RefreshDraft()
{
	if (!Catalog)
	{
		return;
	}
	DraftAppearance = Catalog->NormalizeAppearance(DraftAppearance);
	const FRogue10mCharacterArchetype* Archetype = GetDraftArchetype();
	if (!Archetype)
	{
		return;
	}

	UI_SelectedCharacterInfoText->SetText(FText::Format(
		NSLOCTEXT("Rogue10mCharacterLobby", "CreatingCharacter", "새 캐릭터\n{0} · {1}"),
		RaceLabel(DraftAppearance.Race), GenderLabel(DraftAppearance.Gender)));
	UI_HeadValueText->SetText(IndexedLabel(TEXT("얼굴"), DraftAppearance.HeadStyleIndex, false));
	UI_SkinValueText->SetText(IndexedLabel(TEXT("피부"), DraftAppearance.SkinToneIndex, false));
	UI_HairValueText->SetText(IndexedLabel(TEXT("헤어"), DraftAppearance.HairStyleIndex, true));
	UI_HairColorValueText->SetText(IndexedLabel(TEXT("헤어 색"), DraftAppearance.HairColorIndex, false));
	UI_FacialValueText->SetText(IndexedLabel(TEXT("얼굴 장식"), DraftAppearance.FacialStyleIndex, true));
	UI_EyeValueText->SetText(IndexedLabel(TEXT("눈"), DraftAppearance.EyeColorIndex, false));
	UI_BodyDetailValueText->SetText(IndexedLabel(TEXT("신체 디테일"), DraftAppearance.BodyDetailIndex, true));

	const bool bHasHair = !Archetype->HairMeshes.IsEmpty();
	const bool bHasFacial = !Archetype->FacialMeshes.IsEmpty();
	const bool bHasBodyDetail = !Archetype->BodyDetailMaterials.IsEmpty();
	UI_HairPreviousButton->SetIsEnabled(bHasHair);
	UI_HairNextButton->SetIsEnabled(bHasHair);
	UI_HairColorPreviousButton->SetIsEnabled(bHasHair && !Archetype->HairMaterials.IsEmpty());
	UI_HairColorNextButton->SetIsEnabled(bHasHair && !Archetype->HairMaterials.IsEmpty());
	UI_FacialPreviousButton->SetIsEnabled(bHasFacial);
	UI_FacialNextButton->SetIsEnabled(bHasFacial);
	UI_BodyDetailPreviousButton->SetIsEnabled(bHasBodyDetail);
	UI_BodyDetailNextButton->SetIsEnabled(bHasBodyDetail);
	RefreshPreview(DraftAppearance);
}

void URogue10mCharacterLobbyWidget::RefreshPreview(
	const FRogue10mCharacterAppearance& Appearance)
{
	if (PreviewActor)
	{
		PreviewActor->SetAppearance(Appearance);
	}
}

void URogue10mCharacterLobbyWidget::SelectSlot(int32 SlotIndex)
{
	ClearDeleteConfirmation();
	if (!SlotProfileIds.IsValidIndex(SlotIndex) || !SlotProfileIds[SlotIndex].IsValid())
	{
		BeginCreation();
		return;
	}
	SelectedProfileId = SlotProfileIds[SlotIndex];
	RefreshMode();
}

void URogue10mCharacterLobbyWidget::BeginCreation()
{
	if (!ProfileSubsystem
		|| ProfileSubsystem->GetProfileCount() >= URogue10mCharacterProfileSubsystem::MaximumProfileCount)
	{
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "NoEmptySlots", "빈 캐릭터 슬롯이 없습니다."), true);
		return;
	}
	bCreationMode = true;
	DraftAppearance = FRogue10mCharacterAppearance();
	UI_CharacterNameInput->SetText(FText::GetEmpty());
	ClearDeleteConfirmation();
	RefreshSlotButtons();
	RefreshMode();
	SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "CreationStarted", "새 캐릭터의 이름과 외형을 설정하세요."));
}

void URogue10mCharacterLobbyWidget::EndCreation()
{
	bCreationMode = false;
	RefreshAll();
}

void URogue10mCharacterLobbyWidget::CycleRequired(int32& Value, int32 Count, int32 Delta)
{
	if (Count <= 0)
	{
		Value = 0;
		return;
	}
	Value = (Value + Delta + Count) % Count;
}

void URogue10mCharacterLobbyWidget::CycleOptional(int32& Value, int32 Count, int32 Delta)
{
	if (Count <= 0)
	{
		Value = INDEX_NONE;
		return;
	}
	const int32 OptionCount = Count + 1;
	const int32 Current = Value + 1;
	Value = (Current + Delta + OptionCount) % OptionCount - 1;
}

const FRogue10mCharacterArchetype* URogue10mCharacterLobbyWidget::GetDraftArchetype() const
{
	return Catalog ? Catalog->FindArchetype(DraftAppearance.Race, DraftAppearance.Gender) : nullptr;
}

void URogue10mCharacterLobbyWidget::SetStatus(const FText& Status, bool bError)
{
	UI_StatusText->SetText(Status);
	UI_StatusText->SetColorAndOpacity(bError
		? FSlateColor(FLinearColor(1.0f, 0.35f, 0.3f))
		: FSlateColor(FLinearColor(0.55f, 0.88f, 1.0f)));
}

void URogue10mCharacterLobbyWidget::ClearDeleteConfirmation()
{
	PendingDeleteProfileId.Invalidate();
}

void URogue10mCharacterLobbyWidget::HandleSlot1Clicked() { SelectSlot(0); }
void URogue10mCharacterLobbyWidget::HandleSlot2Clicked() { SelectSlot(1); }
void URogue10mCharacterLobbyWidget::HandleSlot3Clicked() { SelectSlot(2); }
void URogue10mCharacterLobbyWidget::HandleNewCharacterClicked() { BeginCreation(); }

void URogue10mCharacterLobbyWidget::HandleDeleteCharacterClicked()
{
	if (!ProfileSubsystem || !SelectedProfileId.IsValid())
	{
		return;
	}
	if (PendingDeleteProfileId != SelectedProfileId)
	{
		PendingDeleteProfileId = SelectedProfileId;
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "ConfirmDelete", "삭제 버튼을 한 번 더 누르면 캐릭터가 삭제됩니다."), true);
		return;
	}
	if (ProfileSubsystem->DeleteProfile(SelectedProfileId))
	{
		SelectedProfileId.Invalidate();
		ClearDeleteConfirmation();
		RefreshAll();
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "DeleteSuccess", "캐릭터를 삭제했습니다."));
	}
	else
	{
		SetStatus(ProfileSubsystem->GetLastError(), true);
	}
}

void URogue10mCharacterLobbyWidget::HandleEnterGameClicked()
{
	if (!RogueController || !ProfileSubsystem || !SelectedProfileId.IsValid())
	{
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "NoCharacterSelected", "접속할 캐릭터를 선택하세요."), true);
		return;
	}
	if (!ProfileSubsystem->SelectProfile(SelectedProfileId))
	{
		SetStatus(ProfileSubsystem->GetLastError(), true);
		return;
	}
	RogueController->EnterSelectedCharacter();
}

void URogue10mCharacterLobbyWidget::HandleCreateCharacterClicked()
{
	if (!ProfileSubsystem || !Catalog)
	{
		return;
	}
	FRogue10mCharacterProfile CreatedProfile;
	if (!ProfileSubsystem->CreateProfile(
		UI_CharacterNameInput->GetText().ToString(),
		Catalog->NormalizeAppearance(DraftAppearance),
		CreatedProfile))
	{
		SetStatus(ProfileSubsystem->GetLastError(), true);
		return;
	}

	SelectedProfileId = CreatedProfile.ProfileId;
	bCreationMode = false;
	RefreshAll();
	SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "CreateSuccess", "캐릭터를 생성했습니다. 게임 접속을 눌러 주세요."));
}

void URogue10mCharacterLobbyWidget::HandleCancelCreationClicked()
{
	if (ProfileSubsystem && ProfileSubsystem->GetProfileCount() > 0)
	{
		EndCreation();
	}
	else
	{
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "NeedFirstCharacter", "게임을 시작하려면 캐릭터를 하나 생성해야 합니다."), true);
	}
}

void URogue10mCharacterLobbyWidget::HandleRandomizeClicked()
{
	DraftAppearance.Race = static_cast<ERogue10mCharacterRace>(FMath::RandRange(0, 2));
	DraftAppearance.Gender = static_cast<ERogue10mCharacterGender>(FMath::RandRange(0, 1));
	if (const FRogue10mCharacterArchetype* Archetype = GetDraftArchetype())
	{
		DraftAppearance.HeadStyleIndex = FMath::RandRange(0, FMath::Max(0, Archetype->HeadStyleCount - 1));
		DraftAppearance.SkinToneIndex = FMath::RandRange(0, FMath::Max(0, Archetype->SkinToneCount - 1));
		DraftAppearance.HairStyleIndex = FMath::RandRange(INDEX_NONE, Archetype->HairMeshes.Num() - 1);
		DraftAppearance.HairColorIndex = FMath::RandRange(0, FMath::Max(0, Archetype->HairMaterials.Num() - 1));
		DraftAppearance.FacialStyleIndex = FMath::RandRange(INDEX_NONE, Archetype->FacialMeshes.Num() - 1);
		DraftAppearance.EyeColorIndex = FMath::RandRange(0, FMath::Max(0, Archetype->EyeMaterials.Num() - 1));
		DraftAppearance.BodyDetailIndex = FMath::RandRange(INDEX_NONE, Archetype->BodyDetailMaterials.Num() - 1);
	}
	RefreshDraft();
}

void URogue10mCharacterLobbyWidget::HandleHumanRaceClicked() { DraftAppearance.Race = ERogue10mCharacterRace::Human; RefreshDraft(); }
void URogue10mCharacterLobbyWidget::HandleDwarfRaceClicked() { DraftAppearance.Race = ERogue10mCharacterRace::Dwarf; RefreshDraft(); }
void URogue10mCharacterLobbyWidget::HandleOrcRaceClicked() { DraftAppearance.Race = ERogue10mCharacterRace::Orc; RefreshDraft(); }
void URogue10mCharacterLobbyWidget::HandleMaleGenderClicked() { DraftAppearance.Gender = ERogue10mCharacterGender::Male; RefreshDraft(); }
void URogue10mCharacterLobbyWidget::HandleFemaleGenderClicked() { DraftAppearance.Gender = ERogue10mCharacterGender::Female; RefreshDraft(); }

#define ROGUE10M_CYCLE_REQUIRED_HANDLER(Name, Field, CountExpression, Delta) \
	void URogue10mCharacterLobbyWidget::Name() \
	{ \
		if (const FRogue10mCharacterArchetype* Archetype = GetDraftArchetype()) \
		{ \
			CycleRequired(DraftAppearance.Field, (CountExpression), Delta); \
			RefreshDraft(); \
		} \
	}

#define ROGUE10M_CYCLE_OPTIONAL_HANDLER(Name, Field, CountExpression, Delta) \
	void URogue10mCharacterLobbyWidget::Name() \
	{ \
		if (const FRogue10mCharacterArchetype* Archetype = GetDraftArchetype()) \
		{ \
			CycleOptional(DraftAppearance.Field, (CountExpression), Delta); \
			RefreshDraft(); \
		} \
	}

ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleHeadPreviousClicked, HeadStyleIndex, Archetype->HeadStyleCount, -1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleHeadNextClicked, HeadStyleIndex, Archetype->HeadStyleCount, 1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleSkinPreviousClicked, SkinToneIndex, Archetype->SkinToneCount, -1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleSkinNextClicked, SkinToneIndex, Archetype->SkinToneCount, 1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleHairPreviousClicked, HairStyleIndex, Archetype->HairMeshes.Num(), -1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleHairNextClicked, HairStyleIndex, Archetype->HairMeshes.Num(), 1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleHairColorPreviousClicked, HairColorIndex, Archetype->HairMaterials.Num(), -1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleHairColorNextClicked, HairColorIndex, Archetype->HairMaterials.Num(), 1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleFacialPreviousClicked, FacialStyleIndex, Archetype->FacialMeshes.Num(), -1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleFacialNextClicked, FacialStyleIndex, Archetype->FacialMeshes.Num(), 1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleEyePreviousClicked, EyeColorIndex, Archetype->EyeMaterials.Num(), -1)
ROGUE10M_CYCLE_REQUIRED_HANDLER(HandleEyeNextClicked, EyeColorIndex, Archetype->EyeMaterials.Num(), 1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleBodyDetailPreviousClicked, BodyDetailIndex, Archetype->BodyDetailMaterials.Num(), -1)
ROGUE10M_CYCLE_OPTIONAL_HANDLER(HandleBodyDetailNextClicked, BodyDetailIndex, Archetype->BodyDetailMaterials.Num(), 1)

#undef ROGUE10M_CYCLE_REQUIRED_HANDLER
#undef ROGUE10M_CYCLE_OPTIONAL_HANDLER
