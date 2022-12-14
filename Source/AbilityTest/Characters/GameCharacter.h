// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "GameCharacter.generated.h"

class UInputMappingContext;
struct FInputActionValue;
class UAbilityInputConfig;
struct FGameplayTag;
class UBaseAttributeSet;
class UGameplayAbilityBase;

UCLASS()
class ABILITYTEST_API AGameCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameCharacter();

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	bool bIsDeath = false;

protected:
	// INPUT ACTIONS CONFIG//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mappings")
		UAbilityInputConfig* InputConfig;

	// INPUT MAPPINGS //
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mappings")
		UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mappings")
		int32 BaseMappingPriority = 0;

	uint8 TeamID;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameCharacter")
		UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameCharacter")
		UBaseAttributeSet* BaseAttributeSetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameCharacter")
		FGameplayTag FullHealthTag;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;

	// INPUT FUNCTIONS //
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	void AutoTeamID();
	void AddAbilityToUI(TSubclassOf<UGameplayAbilityBase> InAbility);
public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	uint8 GetTeamID() const { return TeamID; }

	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void AquireAbility(TSubclassOf<UGameplayAbility> InAbility);

	UFUNCTION()
		void OnHealthChange(float Health, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameCharacter", meta = (DisplayName = "OnHealthChanged"))
		void BP_OnHealthChange(float Health, float MaxHealth);

	UFUNCTION()
		void OnManaChange(float Mana, float MaxMana);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameCharacter", meta = (DisplayName = "OnManaChanged"))
		void BP_OnManaChange(float Mana, float MaxMana);

	UFUNCTION()
		void OnStrengthChange(float Strength, float MaxStrength);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameCharacter", meta = (DisplayName = "OnStrengthChanged"))
		void BP_OnStrengthChange(float Strength, float MaxStrength);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameCharacter", meta = (DisplayName = "OnDie"))
		void BP_Die();

	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		bool IsSameTeam(AGameCharacter* Other);

	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void AddGameplayTag(FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void RemoveGameplayTag(FGameplayTag& Tag);
};
