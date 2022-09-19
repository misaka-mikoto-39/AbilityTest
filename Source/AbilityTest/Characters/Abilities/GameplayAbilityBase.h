// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityInfo.h"
#include "GameplayAbilityBase.generated.h"

/**
 *
 */
UCLASS()
class ABILITYTEST_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// set in blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityBase")
		UMaterialInstance* UIMaterial;

	UFUNCTION(BlueprintCallable, Category = "AbilityBase")
		FGameplayAbilityInfo GetAbilityInfo();
};
