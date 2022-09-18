// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilityTest/Input/AbilityInputConfig.h"
#include "GameplayTagContainer.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "BaseAttributeSet.h"

// Sets default values
AGameCharacter::AGameCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 900.0f; // The camera follows at this distance behind the character
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetupAttachment(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetupAttachment(CameraBoom, CameraBoom->SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	BaseAttributeSetComp = CreateDefaultSubobject<UBaseAttributeSet>("BaseAttributeSetComp");
	TeamID = 255;
}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (BaseAttributeSetComp)
	{
		BaseAttributeSetComp->OnHealthChange.AddDynamic(this, &AGameCharacter::OnHealthChange);
		BaseAttributeSetComp->OnManaChange.AddDynamic(this, &AGameCharacter::OnManaChange);
		BaseAttributeSetComp->OnStrengthChange.AddDynamic(this, &AGameCharacter::OnStrengthChange);
	}
	AutoTeamID();
	AddGameplayTag(FullHealthTag);
}

void AGameCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);
		}
	}
}

void AGameCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = this;
	AController* CurrentController = Pawn ? Pawn->GetController() : nullptr;

	if (CurrentController)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void AGameCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Values = InputActionValue.Get<FVector2D>();

	if (Values.X != 0.0f)
	{
		this->AddControllerYawInput(Values.X);
	}

	if (Values.Y != 0.0f)
	{
		this->AddControllerPitchInput(Values.Y * -1.0f);
	}
}

void AGameCharacter::AutoTeamID()
{
	if (GetController() && GetController()->IsPlayerController())
	{
		TeamID = 0;
	}
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// There are ways to bind a UInputAction* to a handler function and multiple types of ETriggerEvent that may be of interest.

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag("InputTag.Move")))
		{
			PlayerEnhancedInputComponent->BindAction(IA, ETriggerEvent::Triggered, this, &AGameCharacter::Input_Move);
		}
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag("InputTag.Look.Mouse")))
		{
			PlayerEnhancedInputComponent->BindAction(IA, ETriggerEvent::Triggered, this, &AGameCharacter::Input_LookMouse);
		}
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag("InputTag.Jump")))
		{
			PlayerEnhancedInputComponent->BindAction(IA, ETriggerEvent::Started, this, &ACharacter::Jump);
			PlayerEnhancedInputComponent->BindAction(IA, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
	}
}

UAbilitySystemComponent* AGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}

void AGameCharacter::AquireAbility(TSubclassOf<UGameplayAbility> InAbility)
{
	if (AbilitySystemComp)
	{
		if (HasAuthority() && InAbility)
		{
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(InAbility, 1, 0));
		}
		AbilitySystemComp->InitAbilityActorInfo(this, this);
	}
}

void AGameCharacter::OnHealthChange(float Health, float MaxHealth)
{
	BP_OnHealthChange(Health, MaxHealth);
	if (Health <= 0.0f && !bIsDeath)
	{
		bIsDeath = true;
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->DisableInput(PC);
		}
		else
		{
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Dead");
			}
		}
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		BP_Die();
	}
}

void AGameCharacter::OnManaChange(float Mana, float MaxMana)
{
	BP_OnManaChange(Mana, MaxMana);
}

void AGameCharacter::OnStrengthChange(float Strength, float MaxStrength)
{
	BP_OnStrengthChange(Strength, MaxStrength);
}

bool AGameCharacter::IsSameTeam(AGameCharacter* Other)
{
	return TeamID == Other->GetTeamID();
}

void AGameCharacter::AddGameplayTag(FGameplayTag& Tag)
{
	GetAbilitySystemComponent()->AddLooseGameplayTag(Tag);
	GetAbilitySystemComponent()->SetTagMapCount(Tag, 1); // make it always to 1
}

void AGameCharacter::RemoveGameplayTag(FGameplayTag& Tag)
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(Tag);
}