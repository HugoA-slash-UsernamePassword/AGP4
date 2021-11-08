// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "EngineUtils.h"
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

	//CheckSpawnRoom();
	/*while (CurrentNodes < MaxNodes)
	{
		CurrentNodes++;
		SpawnRoom(LevelPalette);
		if(bSpawnHallways) SpawnRoom(JoinPalette);
	}
	AIManager->BeginAI();*/
}
void ALevelGenerator::SpawnRoom(TArray<TSubclassOf<ALevelData>> palette)
{
	if (palette.Num() == 0) { palette = LevelPalette; UE_LOG(LogTemp, Warning, TEXT("No valid palette selected; defaulting to LevelPalette.")) }
	//DrawDebugPoint(GetWorld(), SpawnPoint.GetLocation(), 10.0f, FColor::Green, true); //Debug spawn point
	TSubclassOf<ALevelData> LevelToSpawn;
	if (RowNum == LastDisplaceNum && LastDisplaceNum != 0) {
		LevelToSpawn = palette[0];
		UE_LOG(LogTemp, Warning, TEXT("Placing room 0"))
	}
	else LevelToSpawn = palette[Seed.RandRange(0, palette.Num() - 1)]; //Random level from selected palette
	ALevelData* NewLevel = GetWorld()->SpawnActor<ALevelData>(LevelToSpawn, SpawnPoint); //Spawn level
	GenerateRoom(NewLevel);
}
void ALevelGenerator::SpawnRoom(TSubclassOf<ALevelData> room)
{
	ALevelData* NewLevel = GetWorld()->SpawnActor<ALevelData>(room, SpawnPoint); //Spawn level
	GenerateRoom(NewLevel);
}
void ALevelGenerator::GenerateRoom(ALevelData* NewLevel)
{
	int diffRowNum = (RowNum + LastRowNum) * FMath::Sign(RowNum);
	bool turnBack = RowNum == LastDisplaceNum && LastDisplaceNum != 999;
	NewLevel->GetComponents<USceneComponent>(nodes); //Get components
	TArray<USceneComponent*> NavPoints; //Location to put navigation nodes
	for (int32 i = nodes.Num() - 1; i >= 0; i--)
	{
		if (nodes[i]->ComponentHasTag("Nav")) NavPoints.Add(nodes[i]);
		if (!nodes[i]->ComponentHasTag("Node"))	nodes.RemoveAt(i); //Remove from list if it is not a level node (doorway)
	}
	TArray<int32> validEntry;
	for (int32 i = nodes.Num() - 1; i >= 0; i--)
	{
		if (diffRowNum <= 0)
		{
			if (NewLevel->dangerNodes.Contains(i)) continue;
		}
		else if (RowDisplace > 800)
		{
			if (NewLevel->dangerNodes.Contains(i)) continue;
		}
		validEntry.Add(i);
	}
	if (validEntry.Num() == 0) { UE_LOG(LogTemp, Error, TEXT("No valid entry found")) return; } //Aborts the function if we run out of level nodes.
	int32 chosenDoorInt = validEntry[Seed.RandRange(0, validEntry.Num() - 1)]; //Random node from available nodes
	//int32 chosenDoorInt = 2;
	USceneComponent* chosenDoor = nodes[chosenDoorInt];
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
	TArray<int32> validDoors;
	//UE_LOG(LogTemp, Warning, TEXT("Row %i"), RowNum);

	for (int32 i = nodes.Num() - 1; i >= 0; i--)
	{
		//if (i == chosenDoorInt) break; //so that we don't choose the same node for entry and exit.
		if (turnBack)
		{
			if (!FMath::IsNearlyZero(nodes[i]->GetComponentRotation().Quaternion().Z, 0.001f)) continue;
		}
		if ((nodes[i]->GetComponentLocation().X - NewLevel->GetActorLocation().X) > 1300) { DisplaceNum = RowNum; RowDisplace += nodes[i]->GetComponentLocation().X - NewLevel->GetActorLocation().X - 1200; }
		if (diffRowNum <= 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("diffrownum bug"))
			if (NewLevel->dangerNodes.Contains(i)) continue;
		}
		else if (RowDisplace > 800)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Rowdisplace bug %f"), RowDisplace)
			if (NewLevel->dangerNodes.Contains(i)) continue;
		}
		else if (LastDisplaceNum == RowNum + FMath::Sign(nodes[i]->GetComponentRotation().Quaternion().Z) && !(FMath::IsNearlyZero(nodes[i]->GetComponentRotation().Quaternion().Z, 0.001f)))
		{
			continue;
		}
		//UE_LOG(LogTemp, Error, TEXT("WAHSAfhszufdi0bvsdf: %f"), RowNum + FMath::Sign(nodes[i]->GetComponentRotation().Quaternion().Z));
		if (nodes[i]->GetComponentRotation().Quaternion().GetAngle() < PI * 0.9f) //Ensures that the rooms never loop back on themselves, preventing most collisions.
		{
			if (nodes[i]->GetComponentRotation() != chosenDoor->GetComponentRotation()) //If 2 doors are on the same wall, they will likely cause collisions etc.
			{
				validDoors.Add(i);
			}
		}
	}
	if (validDoors.Num() == 0) { NewLevel->Destroy(); UE_LOG(LogTemp, Error, TEXT("No valid exit found")) return; }
	int32 chosenDoorInt2 = validDoors[Seed.RandRange(0, validDoors.Num() - 1)];
	chosenDoor = nodes[chosenDoorInt2];
	float doorAngle = chosenDoor->GetComponentRotation().Quaternion().Z;

	if (FMath::IsNearlyZero(doorAngle, 0.001f)) {
		LastRowNum = RowNum;
		LastDisplaceNum = DisplaceNum - RowNum; // -FMath::Sign(RowNum);
		RowNum = 0;
		RowDisplace = 0;
		DisplaceNum = 999;
	}
	else {
		//if (RowNum == 0) RowNum += FMath::Sign(doorAngle);
		//if (DisplaceNum == 0) DisplaceNum += FMath::Sign(doorAngle);
		RowNum += FMath::Sign(doorAngle);
		if (doorAngle < 0) { UE_LOG(LogTemp, Log, TEXT("Going left"), doorAngle); }
		else { UE_LOG(LogTemp, Log, TEXT("Going right"), doorAngle) };
	}
	float newDisplace = chosenDoor->GetComponentLocation().X - nodes[chosenDoorInt]->GetComponentLocation().X;
	if (RowNum != 0) RowDisplace += newDisplace;
	if (newDisplace > 150) {
		DisplaceNum = RowNum;
		RowDisplace += newDisplace;
		UE_LOG(LogTemp, Warning, TEXT("Warning: collision chance"), RowDisplace);
	}
	else if (RowDisplace < 150) DisplaceNum = 999;
	//UE_LOG(LogTemp, Warning, TEXT("Displacement: %f"), RowDisplace);
	//UE_LOG(LogTemp, Warning, TEXT("Row num: %i"), RowNum);
	//UE_LOG(LogTemp, Warning, TEXT("Displacement: %i"), DisplaceNum);

	//UE_LOG(LogTemp, Warning, TEXT("Displacement: %i"), RowDisplace);



	chosenDoor->GetChildComponent(0)->DestroyComponent(); //A doorway will be made on the other side.

	SpawnPoint = chosenDoor->GetComponentTransform();
	//UE_LOG(LogTemp, Warning, TEXT("Spawn degrees: %f"), SpawnPoint.GetRotation().GetAngle()); //debug angle, in case the room loops back on itself.

	TArray<ANavigationNode*> NavPointsNode; //Navigation nodes to be spawned
	for (int32 i = NavPoints.Num() - 1; i >= 0; i--)
	{
		ANavigationNode* _node = GetWorld()->SpawnActor<ANavigationNode>(NodeToSpawn, NavPoints[i]->GetComponentTransform());
		NavPointsNode.Add(_node); //Create nodes based on NavPoints.
	}
	for (size_t i = 0; i < NewLevel->connections.Num(); i++)
	{
		FMapNode MNode = NewLevel->connections[i];
		NavPointsNode[MNode.start]->ConnectedNodes.Add(NavPointsNode[MNode.end]);
		if (!MNode.oneway) NavPointsNode[MNode.end]->ConnectedNodes.Add(NavPointsNode[MNode.start]);
		NavPointsNode[MNode.start]->bIsMainNode = MNode.main;
	}
	if (lastNode != NULL)
	{
		lastNode->ConnectedNodes.Add(NavPointsNode[chosenDoorInt]);
		NavPointsNode[chosenDoorInt]->ConnectedNodes.Add(lastNode);
	}
	lastNode = NavPointsNode[chosenDoorInt2];
	rooms.Add(NewLevel);
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CheckSpawnRoom();
}

void ALevelGenerator::Initialize(int32 seedInt)
{
	Seed = FRandomStream(seedInt);
	UE_LOG(LogTemp, Warning, TEXT("Initial seed: %i"), Seed.GetInitialSeed())
	CheckSpawnRoom();
}

void ALevelGenerator::CheckSpawnRoom()
{
	if (CurrentNodes < MaxNodes)
	{
		SpawnRoom(LevelPalette);
		//if (bSpawnHallways) SpawnRoom(JoinPalette);
		CurrentNodes++;
	}
	else if (!bSpawnedAI)
	{
		AIManager->BeginAI();
		bSpawnedAI = true;
	}
}