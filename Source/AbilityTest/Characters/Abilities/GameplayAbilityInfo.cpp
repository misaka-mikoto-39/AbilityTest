// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilityInfo.h"

FGameplayAbilityInfo::FGameplayAbilityInfo()
	: CooldownDuration(0)
	, Cost(0)
	, CostType(EAbilityCostType::Mana)
	, UIMaterial(nullptr)
	, AbilityClass(nullptr)
{
}

FGameplayAbilityInfo::FGameplayAbilityInfo(float InCooldownDuration, float InCost, EAbilityCostType InCostType, UMaterialInstance* InUIMaterial, TSubclassOf<UGameplayAbilityBase> InAbilityClass)
	: CooldownDuration(InCooldownDuration)
	, Cost(InCost)
	, CostType(InCostType)
	, UIMaterial(InUIMaterial)
	, AbilityClass(InAbilityClass)
{
}