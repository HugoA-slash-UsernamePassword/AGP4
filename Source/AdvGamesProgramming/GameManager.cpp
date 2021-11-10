// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{

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
		Player->UpdateHUD(Player->Score);

		AllEnemies.Remove(Enemy);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player score is: %i"), Player->Score));

		
	}

	UE_LOG(LogTemp, Warning, TEXT("ENEMY DIED"));
}

void AGameManager::PlayerDeath(APlayerCharacter* _Player)
{
	for (APlayerCharacter* Player : AllPlayers)
	{
		Player->Score -= 10;
		Player->UpdateHUD(Player->Score);


		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player score is: %i"), Player->Score));
	}
}

