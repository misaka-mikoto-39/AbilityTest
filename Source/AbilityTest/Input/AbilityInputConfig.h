// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInputConfig.generated.h"

class UInputAction;
/**
 * FAbilityInputAction
 *
 *	Struct used to map a input action to a gameplay input tag.
 */
USTRUCT(BlueprintType)
struct FAbilityInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
		const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
		FGameplayTag InputTag;
};

/**
 * UAbilityInputConfig
 *
 * Non-mutable data asset that contains input configuration properties.
 */
UCLASS()
class ABILITYTEST_API UAbilityInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:

	UAbilityInputConfig(const FObjectInitializer& ObjectInitializer);

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	//const UInputAction* FindAbilityInputActionsForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
		TArray<FAbilityInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
		TArray<FAbilityInputAction> AbilityInputActions;*/
};
