// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set default member variable values
	LookSensitivity = 1.0f;
	NormalMovementSpeed = 600.0f;
	SprintMultiplier = 1.5;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Camera = FindComponentByClass<UCameraComponent>();
	bCanDoubleJump = true;
	SprintMovementSpeed = NormalMovementSpeed * SprintMultiplier;
	//bUseControllerRotationPitch = true;

	HealthComponent = FindComponentByClass<UHealthComponent>();

	if (HealthComponent != nullptr)
	{
		HealthComponent->SetIsReplicated(true);
	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &APlayerCharacter::Strafe);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"),EInputEvent::IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &APlayerCharacter::EndCrouch);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &APlayerCharacter::SprintEnd);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::MoveForward(float Value) 
{
	FRotator ForwardRotation = GetControlRotation();
	ForwardRotation.Roll = 0.0f;
	ForwardRotation.Pitch = 0.0f;
	AddMovementInput(ForwardRotation.Vector(), Value);
}

void APlayerCharacter::Strafe(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
}

void APlayerCharacter::LookUp(float Value) 
{

	FRotator LookUpRotation = FRotator::ZeroRotator;
	LookUpRotation.Pitch = (Value * LookSensitivity);
	if (Camera->RelativeRotation.Pitch + Value * LookSensitivity > 90)
	{
		Camera->RelativeRotation.Pitch = 90;
	}
	else if (Camera->RelativeRotation.Pitch + Value * LookSensitivity < -90) 
	{
		Camera->RelativeRotation.Pitch = -90;
	}
	else 
	{
		Camera->AddRelativeRotation(LookUpRotation);
	}
		Camera->RelativeRotation.Yaw = 0.0f;
		Camera->RelativeRotation.Roll = 0.0f;

}

void APlayerCharacter::Turn(float Value) 
{
	AddControllerYawInput(Value * LookSensitivity);
}

void APlayerCharacter::SprintStart() {
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;

	ServerSprintStart();
}

void APlayerCharacter::ServerSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
}

void APlayerCharacter::SprintEnd() {
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	ServerSprintEnd();
}

void APlayerCharacter::ServerSprintEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
}

void APlayerCharacter::StartCrouch() {
	Super::Crouch();
	Camera->RelativeLocation.Z = 10.0f;
}

void APlayerCharacter::EndCrouch() {
	Super::UnCrouch();
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Camera->RelativeLocation.Z = 45.0f;
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Camera->RelativeLocation.Z = 10.0f;
}

void APlayerCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	bCanDoubleJump = true;
}

void APlayerCharacter::Jump() {
	Super::Jump();
	if (GetMovementComponent()->IsFalling() && bCanDoubleJump) {
		ACharacter::LaunchCharacter(FVector(0, 0, 500), false, true);
		bCanDoubleJump = false;
	}
}

void APlayerCharacter::Reload()
{
	BlueprintReload();
}

void APlayerCharacter::StartJump()
{
	BlueprintJump();
}


