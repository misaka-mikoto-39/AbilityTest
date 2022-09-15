// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BaseAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangeDelegate, float, Health, float, MaxHealth);

/**
 *
 */
UCLASS()
class ABILITYTEST_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData Mana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData MaxStrength;


	FOnAttributeChangeDelegate OnHealthChange;
	FOnAttributeChangeDelegate OnManaChange;
	FOnAttributeChangeDelegate OnStrengthChange;
};
