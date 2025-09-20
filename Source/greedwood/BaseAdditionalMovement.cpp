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

void ABaseAdditionalMovement::ForwardDash()
{
    // TEST: This is a test change to verify compilation
    // Implement dash functionality here
    FVector ForwardVector = GetActorForwardVector();//    SetActorLocation(DashLocation);
    FVector MediumForwardDash = ForwardVector * 4000.0f;
//    LaunchCharacter(ForwardVector * 600.0f, true, true);
    LaunchCharacter(MediumForwardDash, true, true);

}

void ABaseAdditionalMovement::SlowWalk()
{
    // Implement walk functionality here
    // Get current MaxWalkSpeed
    float CurrentMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
    // If current speed is greater than 150, set to 150
    if (CurrentMaxSpeed > 300.0f)
    {
        GetCharacterMovement()->MaxWalkSpeed = 300.0f;
        return;
    }
    else
    {
        // If current speed is less than or equal to 150, reset to default speed (600)
        GetCharacterMovement()->MaxWalkSpeed = 600.0f;
        return;
    }

}

