// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbilityInfo.h"
#include "PlayerControllerBase.generated.h"

/**
 *
 */
UCLASS()
class ABILITYTEST_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerControllerBase")
		void AddAbilityToUI(FGameplayAbilityInfo AbilityInfo);
};
