// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavigationNode.h"
#include "LevelData.h"
#include "AIManager.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();
	UPROPERTY(EditAnywhere)
		int32 MaxNodes;
	int32 CurrentNodes;
	UPROPERTY(EditAnywhere)
		bool bSpawnHallways;
	bool bSpawnedAI;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<ALevelData>> LevelPalette; //Array of level blueprints to be instantiated
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<ALevelData>> JoinPalette; //Currently unused; Array of hallways to be instantiated between each level.
	UPROPERTY(EditAnywhere)
		FTransform SpawnPoint; //Position to spawn the next door. Also the position to spawn the first door.

	TArray<USceneComponent*> nodes; //Temp
	USceneComponent* respawnPoint; //Temp; used if the current room's respawn point cannot be found.
	int RowNum;
	float RowDisplace;
	int LastRowNum;
	int DisplaceNum;
	int LastDisplaceNum;

	UPROPERTY(EditAnywhere)
		UStaticMesh* doorway; //Doorway to add between rooms.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> RespawnPoints;
	UPROPERTY(EditAnywhere)
		TSubclassOf<ANavigationNode> NodeToSpawn; //Temp; Node to spawn. May be used by the new AI system.

	ANavigationNode* lastNode; //The last navigation node spawned, so that the new room can connect to it.

	UPROPERTY(EditAnywhere)
		AAIManager* AIManager; //AIManager. Reference needed to setup navigation grid after all navigation nodes have been spawned.

	FRandomStream Seed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//bool CheckNear(int i);

public:
	UFUNCTION(BlueprintCallable)
		void Initialize(int32 seedInt);
	UFUNCTION(BlueprintCallable)
		void CheckSpawnRoom();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnRoom(TArray<TSubclassOf<ALevelData>> palette);
	void SpawnRoom(TSubclassOf<ALevelData> room);
	void GenerateRoom(ALevelData* NewLevel);
};