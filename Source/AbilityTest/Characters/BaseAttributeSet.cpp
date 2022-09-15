// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameCharacter.h"
#include "GameplayEffect.h"

UBaseAttributeSet::UBaseAttributeSet()
	:Health(200.0f)
	, MaxHealth(200.0f)
	, Mana(100.0f)
	, MaxMana(100.0f)
	, Strength(250.0f)
	, MaxStrength(250.0f)
{
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UBaseAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UBaseAttributeSet, Health)))
	{
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, MaxHealth.GetCurrentValue()));
		Health.SetBaseValue(FMath::Clamp(Health.GetBaseValue(), 0.f, MaxHealth.GetCurrentValue()));
		OnHealthChange.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
		AGameCharacter* Owner = Cast<AGameCharacter>(GetOwningActor());
		if (Health.GetCurrentValue() == MaxHealth.GetCurrentValue())
		{
			if (Owner)
			{
				Owner->AddGameplayTag(Owner->FullHealthTag);
			}
		}
		else
		{
			if (Owner)
			{
				Owner->RemoveGameplayTag(Owner->FullHealthTag);
			}
		}
	}

	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UBaseAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UBaseAttributeSet, Mana)))
	{
		Mana.SetCurrentValue(FMath::Clamp(Mana.GetCurrentValue(), 0.f, MaxMana.GetCurrentValue()));
		Mana.SetBaseValue(FMath::Clamp(Mana.GetBaseValue(), 0.f, MaxMana.GetCurrentValue()));
		OnManaChange.Broadcast(Mana.GetCurrentValue(), MaxMana.GetCurrentValue());
	}

	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UBaseAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UBaseAttributeSet, Strength)))
	{
		Strength.SetCurrentValue(FMath::Clamp(Strength.GetCurrentValue(), 0.f, MaxStrength.GetCurrentValue()));
		Strength.SetBaseValue(FMath::Clamp(Strength.GetBaseValue(), 0.f, MaxStrength.GetCurrentValue()));
		OnStrengthChange.Broadcast(Strength.GetCurrentValue(), MaxStrength.GetCurrentValue());
		UE_LOG(LogClass, Log, TEXT("Strength change, Strength = %f"), Strength.GetCurrentValue());
	}

}