// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.h"
#include "GameManager.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class ADVGAMESPROGRAMMING_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UCameraComponent* Camera;

	UHealthComponent* HealthComponent;

	class AGameManager* GameManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanDoubleJump;
	float ForwardAxis;
	float StrafeAxis;

	
	float SprintMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NormalMovementSpeed;

public:	

	UPROPERTY(EditAnywhere)
	float LookSensitivity;

	UPROPERTY(EditAnywhere)
	float SprintMultiplier;

	UPROPERTY(Replicated)
	int32 Score;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Strafe(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void SprintStart();
	void SprintEnd();

	UFUNCTION(Server, Reliable)
		void ServerSprintStart();

	UFUNCTION(Server, Reliable)
		void ServerSprintEnd();

	void StartCrouch();
	void EndCrouch();
	virtual void OnEndCrouch(
		float HalfHeightAdjust,
		float ScaledHalfHeightAdjust
	) override;
	virtual void OnStartCrouch
	(
		float HalfHeightAdjust,
		float ScaledHalfHeightAdjust
	) override;
	virtual void Landed
	(
		const FHitResult& Hit
	) override;

	virtual void Jump() override;

	void Reload();
	void StartJump();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintReload();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintJump();
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();
};
