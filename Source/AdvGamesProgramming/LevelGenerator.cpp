// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "LevelGenerator.h"
#include "DrawDebugHelpers.h"


// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxNodes = 2;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	while (CurrentNodes < MaxNodes)
	{
		CurrentNodes++;
		SpawnRoom(LevelPalette);
		if(bSpawnHallways) SpawnRoom(JoinPalette);
	}
	AIManager->BeginAI();
}
void ALevelGenerator::SpawnRoom(TArray<TSubclassOf<ALevelData>> palette)
{
	//DrawDebugPoint(GetWorld(), SpawnPoint.GetLocation(), 10.0f, FColor::Green, true); //Debug spawn point
	TSubclassOf<ALevelData> LevelToSpawn = palette[FMath::RandRange(0, palette.Num() - 1)]; //Random level from selected palette
	ALevelData* NewLevel = GetWorld()->SpawnActor<ALevelData>(LevelToSpawn, SpawnPoint); //Spawn level
	NewLevel->GetComponents<USceneComponent>(nodes); //Get components
	TArray<USceneComponent*> NavPoints; //Location to put navigation nodes
	for (int32 i = nodes.Num() - 1; i >= 0; i--)
	{
		if (nodes[i]->ComponentHasTag("Nav")) NavPoints.Add(nodes[i]);
		if (!nodes[i]->ComponentHasTag("Node"))	nodes.RemoveAt(i); //Remove from list if it is not a level node (doorway)
	}
	if (nodes.Num() == 0) return; //Aborts the function if we run out of level nodes.
	int32 chosenDoorInt = FMath::RandRange(0, nodes.Num() - 1); //Random node from available nodes
	USceneComponent* chosenDoor = nodes[chosenDoorInt];
	nodes.RemoveAt(chosenDoorInt); //Remove from array so that we don't choose the same node for entry and exit.
	UStaticMeshComponent* _doorway = Cast<UStaticMeshComponent>(chosenDoor->GetChildComponent(0));
	_doorway->SetStaticMesh(doorway); //Set doorway mesh

	//Apply vectors
	NewLevel->AddActorWorldRotation(chosenDoor->GetComponentRotation().GetInverse()); //Subtract component rotation (and base rotation)
	NewLevel->AddActorWorldRotation(SpawnPoint.GetRotation()); //Add base rotation
	NewLevel->SetActorLocation(chosenDoor->GetComponentLocation()); //Move based on door location. This only works if the rooms are symmetrical.
	NewLevel->AddActorWorldRotation(FQuat(FVector::UpVector, PI)); //rotate by 180 degrees
	
	float LevelHeight = chosenDoor->GetRelativeLocation().Z; 
	NewLevel->AddActorWorldOffset(FVector(0, 0, -2 * LevelHeight)); //invert the door location change
	//UE_LOG(LogTemp, Warning, TEXT("Room height: %f"), LevelHeight) //Debug door height.
	for (int32 i = nodes.Num() - 1; i >= 0; i--)
	{
		if (nodes[i]->GetComponentRotation().Quaternion().GetAngle() > PI * 0.9f) nodes.RemoveAt(i); //Ensures that the rooms never loop back on themselves, preventing most collisions.
		else if (nodes[i]->GetComponentRotation() == chosenDoor->GetComponentRotation()) nodes.RemoveAt(i); //If 2 doors are on the same wall, they will likely cause collisions etc.
	}
	if(nodes.Num() == 0) return;
	int32 chosenDoorInt2 = FMath::RandRange(0, nodes.Num() - 1);
	chosenDoor = nodes[chosenDoorInt2];

	chosenDoor->GetChildComponent(0)->DestroyComponent(); //A doorway will be made on the other side.

	SpawnPoint = chosenDoor->GetComponentTransform();
	//UE_LOG(LogTemp, Warning, TEXT("Spawn degrees: %f"), SpawnPoint.GetRotation().GetAngle()); //debug angle, in case the room loops back on itself.

	TArray<ANavigationNode*> NavPointsNode; //Navigation nodes to be spawned
	for (int32 i = NavPoints.Num() - 1; i >= 0; i--)
	{
		NavPointsNode.Add(GetWorld()->SpawnActor<ANavigationNode>(NodeToSpawn, NavPoints[i]->GetComponentTransform())); //Create nodes based on NavPoints.
	}
	for (size_t i = 0; i < NewLevel->connections.Num(); i++)
	{
		FMapNode MNode = NewLevel->connections[i];
		NavPointsNode[MNode.start]->ConnectedNodes.Add(NavPointsNode[MNode.end]);
		if (!MNode.oneway) NavPointsNode[MNode.end]->ConnectedNodes.Add(NavPointsNode[MNode.start]);
	}
	if (lastNode != NULL)
	{
		lastNode->ConnectedNodes.Add(NavPointsNode[chosenDoorInt]);
		NavPointsNode[chosenDoorInt]->ConnectedNodes.Add(lastNode);
	}
	NavPointsNode.RemoveAt(chosenDoorInt);
	lastNode = NavPointsNode[chosenDoorInt2];
	rooms.Add(NewLevel);
}


// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}