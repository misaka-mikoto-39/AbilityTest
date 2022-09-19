// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilityInfo.generated.h"

class UGameplayAbilityBase;

UENUM(BlueprintType)
enum class EAbilityCostType : uint8
{
	Health,
	Mana,
	Strength
};

USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
	GENERATED_BODY()

public:
	FGameplayAbilityInfo();
	FGameplayAbilityInfo(float InCooldownDuration, float InCost, EAbilityCostType InCostType, UMaterialInstance* InUIMaterial, TSubclassOf<UGameplayAbilityBase> InAbilityClass);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
		float CooldownDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
		float Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
		UMaterialInstance* UIMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
		EAbilityCostType CostType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
		TSubclassOf<UGameplayAbilityBase> AbilityClass;
};