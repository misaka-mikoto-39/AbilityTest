// Fill out your copyright notice in the Description page of Project Settings.


#include "GATargetActorGetAround.h"
#include "Abilities/GameplayAbility.h"

void AGATargetActorGetAround::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
}

void AGATargetActorGetAround::ConfirmTargetingAndContinue()
{
	APawn* OwningPawn = MasterPC->GetPawn();
	if (!OwningPawn)
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
		return;
	}
	FVector PawnLocation = OwningPawn->GetActorLocation();
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
	bool TryOverlap = GetWorld()->OverlapMultiByObjectType(Overlaps, PawnLocation, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(TraceRadius), QueryParams);
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

	if (!OverlapActors.IsEmpty())
	{
		FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlapActors);
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	else
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
}
