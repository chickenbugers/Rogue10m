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
	CharacterPreview,
	SkillTree,
	Settings
};

enum class ERogue10mSkillTreeView : uint8
{
	WeaponSelect,
	WeaponDetail
};

enum class ERogue10mSettingsSlider : uint8
{
	None,
	MasterVolume,
	LookSensitivityX,
	LookSensitivityY
};

class AActor;
class ARogue10mCharacter;
class ARogue10mBasicMonster;
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

struct FRogue10mQuickSlotView
{
	// 플레이어가 눌러야 하는 숫자 키입니다.
	int32 SlotNumber = 1;

	// 슬롯 아래에 작게 표시할 이름입니다.
	FText DisplayName;

	// 실제 아이콘 에셋이 들어오기 전까지 사용할 아이콘 대체 색상입니다.
	FLinearColor IconColor = FLinearColor::White;

	// 슬롯을 사용한 뒤 다시 사용할 수 있을 때까지의 시간입니다.
	float CooldownDuration = 0.0f;

	// 월드 시간 기준 쿨타임 종료 시각입니다.
	float CooldownEndTime = 0.0f;
};

struct FRogue10mSkillWeaponHitArea
{
	// 무기 선택 화면에서 클릭 가능한 무기 카드 영역입니다.
	ERogue10mWeaponType WeaponType = ERogue10mWeaponType::Unarmed;
	FVector2D Position = FVector2D::ZeroVector;
	FVector2D Size = FVector2D::ZeroVector;
};

struct FRogue10mCombatLogEntry
{
	// 화면에 표시할 전투 입력 확인 메시지입니다.
	FString Message;

	// 월드 시간 기준 로그가 사라질 시각입니다.
	float ExpireTime = 0.0f;

	// 로그 종류별 표시 색상입니다.
	FLinearColor Color = FLinearColor::White;
};

struct FRogue10mFloatingDamageEntry
{
	TWeakObjectPtr<AActor> TargetActor;
	float DamageAmount = 0.0f;
	float StartTime = 0.0f;
	float ExpireTime = 0.0f;
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

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	void ToggleSkillTree();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	void ToggleSettings();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void SetInventoryVisible(bool bNewInventoryVisible);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	void SetItemWindowVisible(bool bNewItemWindowVisible);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	void SetSkillTreeVisible(bool bNewSkillTreeVisible);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	void SetSettingsVisible(bool bNewSettingsVisible);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsInventoryVisible() const { return bInventoryVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsItemWindowVisible() const { return bItemWindowVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	bool IsAnyInventoryWindowVisible() const { return bInventoryVisible || bItemWindowVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsAnyBlockingWindowVisible() const { return bInventoryVisible || bItemWindowVisible || bSkillTreeVisible || bSettingsVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Skill Tree")
	bool IsSkillTreeVisible() const { return bSkillTreeVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	bool IsSettingsVisible() const { return bSettingsVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	float GetLookSensitivityX() const { return LookSensitivityX; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	float GetLookSensitivityY() const { return LookSensitivityY; }

	// 숫자 키 입력으로 들어온 퀵 슬롯 사용 요청을 처리합니다.
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	bool ActivateQuickSlot(int32 SlotNumber);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void AddCombatLogMessage(const FString& Message, const FLinearColor& Color = FLinearColor::White, float Duration = 2.0f);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void NotifyPlayerDamaged(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void AddFloatingDamageNumber(AActor* TargetActor, float DamageAmount);

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
	void DrawCharacterInfo();
	void DrawLookedAtMonsterInfo();
	void DrawCombatLog();
	void DrawFloatingDamageNumbers();
	void DrawPlayerDamageFeedback();
	void DrawPanelShortcutHints();
	// 화면 6시 방향에 1~5번 퀵 슬롯 묶음을 그립니다.
	void DrawQuickSlots();

	// 단일 퀵 슬롯의 키, 아이콘 대체색, 이름, 쿨타임 표시를 그립니다.
	void DrawQuickSlot(const FRogue10mQuickSlotView& QuickSlot, const FVector2D& Position, float SlotSize);

	// 현재 월드 시간을 기준으로 남은 쿨타임을 계산합니다.
	float GetQuickSlotCooldownRemaining(const FRogue10mQuickSlotView& QuickSlot) const;
	void DrawInventory();
	void DrawItemWindow();
	void DrawSkillTree();
	void DrawSkillTreeWeaponSelect(float PanelX, float PanelY, float PanelWidth, float PanelHeight);
	void DrawSkillTreeWeaponDetail(float PanelX, float PanelY, float PanelWidth, float PanelHeight);
	void DrawSettingsPanel();
	void DrawSettingsSlider(const FString& Label, ERogue10mSettingsSlider SliderType, float Value, float MinValue, float MaxValue, const FVector2D& Position, float Width);
	void DrawSettingsFpsButton(int32 FpsValue, const FVector2D& Position, const FVector2D& Size);
	void DrawInventorySlots(const TArray<struct FRogue10mInventorySlot>& Slots, float X, float Y, float SlotSize, float Gap, bool bRightSide);
	void DrawItemGrid(class URogue10mInventoryComponent* InventoryComponent, float X, float Y, float SlotSize, float Gap, float MaxWidth, float MaxHeight);
	void DrawHoveredItemTooltip();
	void DrawDraggedItem();
	void DrawCharacterPreview(const ARogue10mCharacter* RogueCharacter, float X, float Y, float Width, float Height);
	void UpdateWindowDrag();
	void UpdateInventoryCursor() const;
	bool EnsureCharacterPreview(const ARogue10mCharacter* RogueCharacter, int32 RenderTargetWidth, int32 RenderTargetHeight);
	void UpdateCharacterPreview(const ARogue10mCharacter* RogueCharacter);
	ARogue10mBasicMonster* FindLookedAtMonster() const;
	FString GetItemCategoryText(ERogue10mItemCategory Category) const;
	FString GetEquipmentSlotText(ERogue10mInventorySlotType SlotType) const;
	// 캐릭터 정보 패널과 아이템 툴팁에서 표시할 무기 타입 문자열을 반환합니다.
	FString GetWeaponTypeText(ERogue10mWeaponType WeaponType) const;
	FLinearColor GetWeaponTypeColor(ERogue10mWeaponType WeaponType) const;
	const FRogue10mEquipmentSlotHitArea* FindEquipmentSlotHitArea(const FVector2D& MousePosition) const;
	const FRogue10mItemSlotHitArea* FindItemSlotHitArea(const FVector2D& MousePosition) const;
	const FRogue10mSkillWeaponHitArea* FindSkillWeaponHitArea(const FVector2D& MousePosition) const;
	bool IsItemCompatibleWithSlot(const FRogue10mItemStack& Item, ERogue10mInventorySlotType SlotType) const;
	bool IsPointInRect(const FVector2D& Point, const FVector2D& RectPosition, const FVector2D& RectSize) const;
	FVector2D ClampWindowPosition(const FVector2D& Position, const FVector2D& Size) const;
	void UpdateSettingsInteraction();
	void UpdateSettingsSliderValue(ERogue10mSettingsSlider SliderType, float LocalAlpha);
	void ApplyFpsLimit(int32 FpsValue);
	void ApplyAudioVolume() const;

	bool bInventoryVisible = false;
	bool bItemWindowVisible = false;
	bool bSkillTreeVisible = false;
	bool bSettingsVisible = false;
	ERogue10mDraggedWindow DraggedWindow = ERogue10mDraggedWindow::None;
	ERogue10mSettingsSlider DraggedSettingsSlider = ERogue10mSettingsSlider::None;
	FVector2D DragOffset = FVector2D::ZeroVector;
	FVector2D InventoryWindowPosition = FVector2D(-1.0f, -1.0f);
	FVector2D ItemWindowPosition = FVector2D(-1.0f, -1.0f);
	FVector2D SkillTreeWindowPosition = FVector2D(-1.0f, -1.0f);
	FVector2D SettingsWindowPosition = FVector2D(-1.0f, -1.0f);
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
	TArray<FRogue10mQuickSlotView> QuickSlots;
	TArray<FRogue10mSkillWeaponHitArea> SkillWeaponHitAreas;
	TArray<FRogue10mCombatLogEntry> CombatLogEntries;
	TArray<FRogue10mFloatingDamageEntry> FloatingDamageEntries;
	ERogue10mSkillTreeView SkillTreeView = ERogue10mSkillTreeView::WeaponSelect;
	ERogue10mWeaponType SelectedSkillTreeWeapon = ERogue10mWeaponType::Unarmed;
	float MasterVolume = 1.0f;
	float LookSensitivityX = 1.0f;
	float LookSensitivityY = 1.0f;
	int32 CurrentFpsLimit = 120;
	float PlayerDamageFeedbackEndTime = 0.0f;
	float PlayerDamageFeedbackStrength = 0.0f;

	UPROPERTY(Transient)
	TObjectPtr<AActor> CharacterPreviewActor;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> CharacterPreviewMesh;

	UPROPERTY(Transient)
	TObjectPtr<USceneCaptureComponent2D> CharacterPreviewCapture;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> CharacterPreviewRenderTarget;
};
