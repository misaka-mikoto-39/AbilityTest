// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilityBase.h"

FGameplayAbilityInfo UGameplayAbilityBase::GetAbilityInfo()
{
	float CooldownDuration = 0;
	float Cost = 0;
	EAbilityCostType CostType = EAbilityCostType::Mana;
	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (CooldownEffect && CostEffect)
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
		if (!CostEffect->Modifiers.IsEmpty())
		{
			FGameplayModifierInfo EffectInfo = CostEffect->Modifiers[0];
			EffectInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
			FGameplayAttribute CostAttr = EffectInfo.Attribute;
			FString AttributeName = CostAttr.AttributeName;
			if (AttributeName == "Health")
			{
				CostType = EAbilityCostType::Health;
			}
			else if (AttributeName == "Mana")
			{
				CostType = EAbilityCostType::Mana;
			}
			else if (AttributeName == "Strength")
			{
				CostType = EAbilityCostType::Strength;
			}
		}
	}
	return FGameplayAbilityInfo(CooldownDuration, Cost, CostType, UIMaterial, GetClass());
}