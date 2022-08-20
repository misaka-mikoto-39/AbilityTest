// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityInputConfig.h"
#include "InputAction.h"

UAbilityInputConfig::UAbilityInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UAbilityInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAbilityInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogClass, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}