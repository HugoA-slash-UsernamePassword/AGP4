// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() != ENetRole::ROLE_Authority) Destroy();
	
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameManager::EnemyDeath(AEnemyCharacter* Enemy)
{
	for (APlayerCharacter* Player : AllPlayers)
	{
		Player->Score += 5;

		AllEnemies.Remove(Enemy);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player score is: %i"), Player->Score));
	}
}

void AGameManager::PlayerDeath(APlayerCharacter* Player)
{
	for (APlayerCharacter* Player : AllPlayers)
	{
		Player->Score -= 10;

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player score is: %i"), Player->Score));
	}
}

