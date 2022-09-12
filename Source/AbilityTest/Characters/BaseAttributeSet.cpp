// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"

UBaseAttributeSet::UBaseAttributeSet()
	:Health(200.0f),
	MaxHealth(200.0f)
{
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UBaseAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UBaseAttributeSet, Health)))
	{
		OnHealthChange.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
		UE_LOG(LogClass, Log, TEXT("Take damage, health = %f"), Health.GetCurrentValue());
	}
}