// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacterLobbyWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Rogue10m.h"
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
	if (UI_LobbyBackgroundImage)
	{
		UI_LobbyBackgroundImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (UI_LobbyBackdrop)
	{
		UI_LobbyBackdrop->SetVisibility(ESlateVisibility::Collapsed);
	}
	for (UWidget* DecorativeWidget :
		{ static_cast<UWidget*>(UI_BackgroundShade.Get()),
			static_cast<UWidget*>(UI_BottomBar.Get()) })
	{
		if (DecorativeWidget)
		{
			DecorativeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	for (UImage* PreviewImage :
		{ UI_CharacterPreviewImage.Get(), UI_Slot1PreviewImage.Get(),
			UI_Slot2PreviewImage.Get(), UI_Slot3PreviewImage.Get() })
	{
		if (PreviewImage)
		{
			PreviewImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	for (UButton* ActionButton :
		{ UI_Slot1Button.Get(), UI_Slot2Button.Get(), UI_Slot3Button.Get(),
			UI_NewCharacterButton.Get(), UI_DeleteCharacterButton.Get(),
			UI_EnterGameButton.Get() })
	{
		if (ActionButton)
		{
			if (UCanvasPanelSlot* CanvasSlot =
				Cast<UCanvasPanelSlot>(ActionButton->Slot))
			{
				CanvasSlot->SetZOrder(8);
			}
		}
	}

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
		UMaterialInterface* PreviewMaterialBase =
			PreviewTransparencyMaterial.LoadSynchronous();
		if (!PreviewMaterialBase)
		{
			UE_LOG(
				LogRogue10m, Error,
				TEXT("Character preview transparency material is missing: %s"),
				*PreviewTransparencyMaterial.ToSoftObjectPath().ToString());
		}

		auto CreatePreview = [this, PreviewMaterialBase](
			UImage* TargetImage,
			FIntPoint Resolution,
			UMaterialInstanceDynamic*& OutMaterial)
		{
			OutMaterial = nullptr;
			ARogue10mCharacterCustomizationPreviewActor* Actor =
				GetWorld()->SpawnActor<ARogue10mCharacterCustomizationPreviewActor>();
			if (!Actor || !Actor->InitializePreview(Catalog, Resolution))
			{
				if (Actor)
				{
					Actor->Destroy();
				}
				return static_cast<ARogue10mCharacterCustomizationPreviewActor*>(nullptr);
			}

			UObject* BrushResource = Actor->GetRenderTarget();
			if (PreviewMaterialBase)
			{
				OutMaterial = UMaterialInstanceDynamic::Create(
					PreviewMaterialBase, this);
				OutMaterial->SetTextureParameterValue(
					TEXT("PreviewTexture"), Actor->GetRenderTarget());
				BrushResource = OutMaterial;
			}

			FSlateBrush Brush;
			Brush.SetResourceObject(BrushResource);
			Brush.ImageSize = FVector2D(Resolution);
			Brush.DrawAs = ESlateBrushDrawType::Image;
			TargetImage->SetBrush(Brush);
			Actor->SetPreviewActive(false);
			return Actor;
		};

		UMaterialInstanceDynamic* CreatedMaterial = nullptr;
		PreviewActor = CreatePreview(
			UI_CharacterPreviewImage,
			FIntPoint(720, 900),
			CreatedMaterial);
		DraftPreviewMaterial = CreatedMaterial;

		UImage* SlotImages[] =
		{
			UI_Slot1PreviewImage,
			UI_Slot2PreviewImage,
			UI_Slot3PreviewImage
		};
		SlotPreviewActors.Reset();
		SlotPreviewMaterials.Reset();
		for (UImage* SlotImage : SlotImages)
		{
			CreatedMaterial = nullptr;
			SlotPreviewActors.Add(CreatePreview(
				SlotImage, FIntPoint(520, 760), CreatedMaterial));
			SlotPreviewMaterials.Add(CreatedMaterial);
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
	for (ARogue10mCharacterCustomizationPreviewActor* SlotPreviewActor : SlotPreviewActors)
	{
		if (SlotPreviewActor)
		{
			SlotPreviewActor->Destroy();
		}
	}
	SlotPreviewActors.Reset();
	DraftPreviewMaterial = nullptr;
	SlotPreviewMaterials.Reset();
	Super::NativeDestruct();
}

FReply URogue10mCharacterLobbyWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		int32 PreviewSlotIndex = INDEX_NONE;
		if (TryGetPreviewSlotAtScreenPosition(
			InMouseEvent.GetScreenSpacePosition(), PreviewSlotIndex))
		{
			SelectSlot(PreviewSlotIndex);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply URogue10mCharacterLobbyWidget::NativeOnMouseButtonDoubleClick(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		int32 PreviewSlotIndex = INDEX_NONE;
		if (TryGetPreviewSlotAtScreenPosition(
			InMouseEvent.GetScreenSpacePosition(), PreviewSlotIndex))
		{
			EnterSlotCharacter(PreviewSlotIndex);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}
void URogue10mCharacterLobbyWidget::RefreshAll()
{
	RefreshSlotButtons();
	if (!ProfileSubsystem || !Catalog)
	{
		SetStatus(NSLOCTEXT("Rogue10mCharacterLobby", "InitializationFailed", "캐릭터 로비 데이터를 초기화하지 못했습니다."), true);
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
				NSLOCTEXT("Rogue10mCharacterLobby", "EmptySlot", "슬롯 {0}\n캐릭터 생성하기"),
				FText::AsNumber(Index + 1)));
		}
		Buttons[Index]->SetIsEnabled(!bCreationMode);
	}
}

void URogue10mCharacterLobbyWidget::RefreshMode()
{
	UI_CreationPanel->SetVisibility(
		bCreationMode ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UI_SelectionStagePanel->SetVisibility(
		bCreationMode ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	UI_CharacterPreviewImage->SetVisibility(
		bCreationMode ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
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
		RefreshStagePreviews();
		RefreshDraft();
		return;
	}

	const FRogue10mCharacterProfile* Profile =
		ProfileSubsystem ? ProfileSubsystem->FindProfile(SelectedProfileId) : nullptr;
	if (Profile)
	{
		RefreshPreview(Profile->Appearance);
	}
	RefreshStagePreviews();
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
		PreviewActor->SetPreviewActive(true);
	}
}

void URogue10mCharacterLobbyWidget::RefreshStagePreviews()
{
	const TArray<FRogue10mCharacterProfile>* Profiles =
		ProfileSubsystem ? &ProfileSubsystem->GetProfiles() : nullptr;
	UImage* SlotImages[] =
	{
		UI_Slot1PreviewImage,
		UI_Slot2PreviewImage,
		UI_Slot3PreviewImage
	};
	UButton* SlotButtons[] =
	{
		UI_Slot1Button,
		UI_Slot2Button,
		UI_Slot3Button
	};

	if (PreviewActor)
	{
		PreviewActor->SetPreviewActive(bCreationMode);
	}

	for (int32 Index = 0; Index < URogue10mCharacterProfileSubsystem::MaximumProfileCount; ++Index)
	{
		ARogue10mCharacterCustomizationPreviewActor* SlotActor =
			SlotPreviewActors.IsValidIndex(Index) ? SlotPreviewActors[Index].Get() : nullptr;
		const bool bHasProfile = !bCreationMode && Profiles && Profiles->IsValidIndex(Index);
		SlotImages[Index]->SetVisibility(
			bHasProfile ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

		if (!SlotActor)
		{
			continue;
		}
		SlotActor->SetPreviewActive(false);
		if (!bHasProfile)
		{
			continue;
		}

		const FRogue10mCharacterProfile& Profile = (*Profiles)[Index];
		if (SlotActor->SetAppearance(Profile.Appearance))
		{
			SlotActor->SetPreviewActive(true);
		}
		const bool bSelected = Profile.ProfileId == SelectedProfileId;
		SlotImages[Index]->SetColorAndOpacity(
			bSelected
				? FLinearColor::White
				: FLinearColor(0.55f, 0.62f, 0.72f, 0.88f));
		SlotButtons[Index]->SetBackgroundColor(
			bSelected
				? FLinearColor(0.2f, 0.62f, 0.9f, 1.0f)
				: FLinearColor(0.035f, 0.055f, 0.08f, 0.92f));
	}
}

bool URogue10mCharacterLobbyWidget::TryGetPreviewSlotAtScreenPosition(
	const FVector2D& ScreenPosition, int32& OutSlotIndex) const
{
	OutSlotIndex = INDEX_NONE;
	if (bCreationMode)
	{
		return false;
	}

	UImage* SlotImages[] =
	{
		UI_Slot1PreviewImage,
		UI_Slot2PreviewImage,
		UI_Slot3PreviewImage
	};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(SlotImages); ++Index)
	{
		if (SlotImages[Index] && SlotImages[Index]->IsVisible()
			&& SlotImages[Index]->GetCachedGeometry().IsUnderLocation(ScreenPosition))
		{
			OutSlotIndex = Index;
			return true;
		}
	}
	return false;
}

void URogue10mCharacterLobbyWidget::SelectSlot(int32 SlotIndex)
{
	ClearDeleteConfirmation();
	if (!SlotProfileIds.IsValidIndex(SlotIndex) || !SlotProfileIds[SlotIndex].IsValid())
	{
		BeginCreation();
		return;
	}

	const double ClickTime = FPlatformTime::Seconds();
	const bool bEnterOnDoubleClick = LastClickedSlotIndex == SlotIndex
		&& ClickTime - LastSlotClickTime <= CharacterEnterDoubleClickSeconds;
	LastClickedSlotIndex = SlotIndex;
	LastSlotClickTime = ClickTime;
	SelectedProfileId = SlotProfileIds[SlotIndex];
	RefreshMode();
	if (bEnterOnDoubleClick)
	{
		HandleEnterGameClicked();
	}
}

void URogue10mCharacterLobbyWidget::EnterSlotCharacter(int32 SlotIndex)
{
	if (!SlotProfileIds.IsValidIndex(SlotIndex) || !SlotProfileIds[SlotIndex].IsValid())
	{
		return;
	}
	SelectedProfileId = SlotProfileIds[SlotIndex];
	RefreshMode();
	HandleEnterGameClicked();
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
	if (bError)
	{
		UE_LOG(LogRogue10m, Warning, TEXT("Character Lobby: %s"), *Status.ToString());
	}
	else
	{
		UE_LOG(LogRogue10m, Log, TEXT("Character Lobby: %s"), *Status.ToString());
	}
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
