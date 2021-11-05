// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "HealthComponent.h"
#include "EnemyCharacter.generated.h"

UENUM()
enum class AgentState : uint8
{
	PATROL,
	ENGAGE,
	EVADE
};

UCLASS()
class ADVGAMESPROGRAMMING_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
	AEnemyCharacter(AEnemyCharacter* SuperiorAI);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere)
	TArray <class ANavigationNode* > Path;

	UPROPERTY(VisibleAnywhere)
	ANavigationNode* CurrentNode;
	UPROPERTY(VisibleAnywhere)
	ANavigationNode* LastNode;
	class AAIManager* Manager;
	float PathfindingNodeAccuracy;

	UPROPERTY(EditAnywhere, meta = (UIMin = "10.0", UIMax = "1000.0", ClampMin = "10.0", ClampMax = "1000.0"))
	float PathfindingEvasionAccuracy;

	UPROPERTY(EditAnywhere, meta = (UIMin = "10.0", UIMax = "1000.0", ClampMin = "10.0", ClampMax = "1000.0"))
	float PathfindingDestinationAccuracy;


	class UHealthComponent* HealthComponent;

	AEnemyCharacter* Superior;
	AEnemyCharacter* Inferior;
	UPROPERTY(VisibleAnywhere)
	float TravelTimer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxTimeBeforeReroute;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	AgentState CurrentAgentState;

	class UAIPerceptionComponent* PerceptionComponent;
	AActor* DetectedActor;
	bool bCanSeeActor;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void AgentPatrol();
	void AgentEngage();
	void AgentEvade();

	UFUNCTION()
	void SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent)
	void Fire(FVector FireDirection);

private:

	void MoveAlongPath();

};
