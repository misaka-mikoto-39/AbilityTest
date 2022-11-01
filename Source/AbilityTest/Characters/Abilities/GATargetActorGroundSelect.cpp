// Fill out your copyright notice in the Description page of Project Settings.


#include "GATargetActorGroundSelect.h"
#include "Components/DecalComponent.h"
#include "Abilities/GameplayAbility.h"

AGATargetActorGroundSelect::AGATargetActorGroundSelect()
{
	PrimaryActorTick.bCanEverTick = true;
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);
	Decal->SetupAttachment(RootComponent);
	TraceRadius = 200.0f;
	Decal->DecalSize = FVector(TraceRadius);
}

void AGATargetActorGroundSelect::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
	Decal->DecalSize = FVector(TraceRadius);
}

void AGATargetActorGroundSelect::ConfirmTargetingAndContinue()
{
	FVector ViewLocation;
	GetPlayerLookingPoint(ViewLocation);
	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>> OverlapActors;
	bool TraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = TraceComplex;
	QueryParams.bReturnPhysicalMaterial = false;
	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		QueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}
	bool TryOverlap = GetWorld()->OverlapMultiByObjectType(Overlaps, ViewLocation, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(TraceRadius), QueryParams);
	if (TryOverlap)
	{
		for (int32 i = 0; i < Overlaps.Num(); ++i)
		{
			APawn* PawnOverlap = Cast<APawn>(Overlaps[i].GetActor());
			if (PawnOverlap && !OverlapActors.Contains(PawnOverlap))
			{
				OverlapActors.AddUnique(PawnOverlap);
			}
		}
	}

	FGameplayAbilityTargetData_LocationInfo* CenterLocation = new FGameplayAbilityTargetData_LocationInfo();
	if (Decal)
	{
		CenterLocation->TargetLocation.LiteralTransform = Decal->GetComponentTransform();
		CenterLocation->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	}

	if (!OverlapActors.IsEmpty())
	{
		FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlapActors);
		TargetData.Add(CenterLocation);
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle(CenterLocation));
	}
}

void AGATargetActorGroundSelect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FVector LookPoint;
	GetPlayerLookingPoint(LookPoint);
	Decal->SetWorldLocation(LookPoint);
}

bool AGATargetActorGroundSelect::GetPlayerLookingPoint(FVector& OutViewPoint)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	MasterPC->GetPlayerViewPoint(ViewPoint, ViewRotation);
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		QueryParams.AddIgnoredActor(MasterPawn->GetUniqueID());
	}
	bool TryTrace = GetWorld()->LineTraceSingleByChannel(HitResult, ViewPoint, ViewPoint + ViewRotation.Vector() * 10000.0f, ECC_Visibility, QueryParams);
	if (TryTrace)
	{
		OutViewPoint = HitResult.ImpactPoint;
	}
	else
	{
		OutViewPoint = FVector();
	}
	return TryTrace;
}
