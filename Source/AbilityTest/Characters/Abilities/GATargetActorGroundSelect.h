// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATargetActorGroundSelect.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYTEST_API AGATargetActorGroundSelect : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
protected:
	// ExposeOnSpawn allow change in spawn param
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	float TraceRadius;
public:
	AGATargetActorGroundSelect();
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable)
		bool GetPlayerLookingPoint(FVector& OutViewPoint);
};
