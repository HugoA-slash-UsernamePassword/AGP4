// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIManager.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();
	void BeginAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	UPROPERTY(EditAnywhere, Category = "AI Properties")
	int32 NumAI;
	int32 NumAISpawned;
	UPROPERTY(EditAnywhere, Category = "AI Properties")
	int32 NumAISquad;
	int32 NumAISquadSpawned;
	UPROPERTY(EditAnywhere, Category = "AI Properties")
	int32 AISquadMaxSize;
	UPROPERTY(EditAnywhere, Category = "AI Properties")
	int32 AISquadMinSize;
	UPROPERTY(VisibleAnywhere, Category = "Navigation Nodes")
	TArray<class ANavigationNode*> AllNodes;
	UPROPERTY(VisibleAnywhere, Category = "Navigation Nodes")
	TArray<class ANavigationNode*> MainNodes;
	UPROPERTY(VisibleAnywhere, Category = "Agents")
	TArray<class AEnemyCharacter*> AllAgents;
	UPROPERTY(EditAnywhere, Category = "Agents")
	TSubclassOf<AEnemyCharacter> AgentToSpawn;

	UPROPERTY(EditAnywhere)
	float AllowedAngle;

	bool bAIStarted;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<ANavigationNode*> GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode);
	void PopulateNodes();
	void CreateAgents();
	void CreateSquad(int32 SquadMemberCount);
	void CreateSingleAI();

	/**
	Finds the nearest navigation node from the given location.
	@param Location - The location that you want to find the nearest node from.
	@return NearestNode - The nearest node to the given location.
	*/
	ANavigationNode* FindNearestNode(const FVector& Location);
	/**
	Finds the furthest navigation node from the given location.
	@param Location - The location that you want to find the furthest node from.
	@return FurthestNode - The furthest node from the given location.
	*/
	ANavigationNode* FindFurthestNode(const FVector& Location);

	void GenerateNodes(const TArray<FVector>& Vertices, int32 Width, int32 Height);
	void AddConnection(ANavigationNode* FromNode, ANavigationNode* ToNode);

private:

	TArray<ANavigationNode*> ReconstructPath(ANavigationNode* StartNode, ANavigationNode* EndNode);
};
