// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BaseAttributeSet.generated.h"

/**
 *
 */
UCLASS()
class ABILITYTEST_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseAttributeSet")
		FGameplayAttributeData Health;

	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
