// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "GameManager.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();
	TArray < class AEnemyCharacter* > AllEnemies;
	TArray < class APlayerCharacter* > AllPlayers;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EnemyDeath(AEnemyCharacter* Enemy);
	UFUNCTION(BlueprintImplementableEvent)
	void BPEnemyDeath();
	void PlayerDeath(APlayerCharacter* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void BPPlayerDeath();

};
