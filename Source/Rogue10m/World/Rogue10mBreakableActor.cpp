// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mBreakableActor.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Rogue10m.h"

ARogue10mBreakableActor::ARogue10mBreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetCanBeDamaged(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Breakable Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ARogue10mBreakableActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && !bBroken)
	{
		CurrentHealth = FMath::Max(1.0f, MaxHealth);
	}
}

float ARogue10mBreakableActor::TakeDamage(
	float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bBroken || AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - AppliedDamage, 0.0f, MaxHealth);
	UE_LOG(LogRogue10m, Log, TEXT("%s 오브젝트 피해 %.1f, 내구도 %.1f / %.1f"),
		*GetNameSafe(this), AppliedDamage, CurrentHealth, MaxHealth);
	if (CurrentHealth <= 0.0f)
	{
		Break(DamageCauser);
	}
	return AppliedDamage;
}

void ARogue10mBreakableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARogue10mBreakableActor, CurrentHealth);
	DOREPLIFETIME(ARogue10mBreakableActor, bBroken);
}

bool ARogue10mBreakableActor::CanReceiveRogue10mAttack_Implementation(AActor* AttackSource) const
{
	return !bBroken && AttackSource != this;
}

FVector ARogue10mBreakableActor::GetRogue10mDamageIndicatorLocation_Implementation() const
{
	FVector BoundsOrigin;
	FVector BoundsExtent;
	GetActorBounds(true, BoundsOrigin, BoundsExtent);
	return BoundsOrigin + FVector(0.0f, 0.0f, BoundsExtent.Z + 20.0f);
}

float ARogue10mBreakableActor::GetHealthNormalized() const
{
	return MaxHealth > 0.0f ? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
}

void ARogue10mBreakableActor::Break(AActor* DamageCauser)
{
	if (bBroken)
	{
		return;
	}
	bBroken = true;
	SetCanBeDamaged(false);
	ApplyBrokenState();
	BP_OnBroken(DamageCauser);
}

void ARogue10mBreakableActor::ApplyBrokenState()
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (bHideWhenBroken)
	{
		MeshComponent->SetVisibility(false, true);
	}
}

void ARogue10mBreakableActor::OnRep_Broken()
{
	if (bBroken)
	{
		ApplyBrokenState();
	}
}
