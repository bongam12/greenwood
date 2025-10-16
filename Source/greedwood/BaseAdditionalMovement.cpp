// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAdditionalMovement.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseAdditionalMovement::ABaseAdditionalMovement()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	



}

// Called when the game starts or when spawned
void ABaseAdditionalMovement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAdditionalMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseAdditionalMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseAdditionalMovement::LoadCharacterPreset()
{
    // Load character preset here - set to normal walk speed (300)
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;
    UE_LOG(LogTemp, Warning, TEXT("***********Character Preset Loaded - Speed set to 300**************"));
}

void ABaseAdditionalMovement::ForwardDash()
{
    // TEST: This is a test change to verify compilation
    // Implement dash functionality here
    FVector DashVector = GetVelocity().GetSafeNormal();
    if (DashVector.IsNearlyZero())
    {
        DashVector = GetActorForwardVector();
    }

    FVector MediumForwardDash = DashVector * 200.0f;
    // Only dash if character is on the ground (not falling)
    if (!GetCharacterMovement()->IsFalling())
    {
       LaunchCharacter(MediumForwardDash, true, true);
    }

}

void ABaseAdditionalMovement::SlowWalk(bool bIsSlowWalking)
{
    // Implement walk functionality here
    // bSlowWalkState is set from Blueprint before calling this function

	if (bIsSlowWalking)
	{
		// When sprint is active, increase walk speed
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		UE_LOG(LogTemp, Warning, TEXT("SlowWalk ACTIVE - Speed set to 300"));
	}
	else
	{
		// When slow walk is not active, use normal walk speed
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
		UE_LOG(LogTemp, Warning, TEXT("SlowWalk INACTIVE - Speed set to 200"));
	}

}


