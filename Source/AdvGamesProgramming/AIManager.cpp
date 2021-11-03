// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "EngineUtils.h"
#include "NavigationNode.h"
#include "EnemyCharacter.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AllowedAngle = 0.4f;
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
}

void AAIManager::BeginAI()
{
	if (AllNodes.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("POPULATING NODES"))
			PopulateNodes();
	}
	CreateAgents();
	UE_LOG(LogTemp, Warning, TEXT("Number of nodes: %i"), AllNodes.Num())
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

TArray<ANavigationNode*> AAIManager::GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	// Create the open set array and add the start node
	TArray<ANavigationNode*> OpenSet;
	OpenSet.Add(StartNode);

	// Set all the GScores to infinity
	for (auto It = AllNodes.CreateIterator(); It; ++It)
	{
		(*It)->GScore = TNumericLimits<float>::Max();
	}

	// Set start node GScore to zero and update the heuristic
	StartNode->GScore = 0.0f;
	StartNode->HScore = FVector::Dist(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	ANavigationNode* CurrentNode;

	// Loop through the open set until it is empty
	while (OpenSet.Num() > 0)
	{
		// Find the node with the lowest FScore
		ANavigationNode* BestNode = OpenSet[0];
		for (auto It = OpenSet.CreateConstIterator(); It; ++It)
		{
			// If the best node is the current node then skip
			if (*It == BestNode) continue;
			// If the current nodes fscore is better than the 
			// best nodes fscore then set the current node as the best node
			if ((*It)->FScore() < BestNode->FScore())
			{
				BestNode = *It;
			}
		}
		// When this loop finishes, the BestNode will be the node with the lowest FScore in the open set
		CurrentNode = BestNode;
		OpenSet.Remove(CurrentNode);

		// If the current node is the end node then we have the path and should reconstruct it
		if (CurrentNode == EndNode)
		{
			return ReconstructPath(StartNode, EndNode);
		}

		// Loop through all of the connected nodes of the current node
		for (auto It = CurrentNode->ConnectedNodes.CreateIterator(); It; ++It)
		{
			float TentativeGScore = CurrentNode->GScore + FVector::Dist(CurrentNode->GetActorLocation(), (*It)->GetActorLocation());
			if (TentativeGScore < (*It)->GScore)
			{
				(*It)->CameFrom = CurrentNode;
				(*It)->GScore = TentativeGScore;
				(*It)->HScore = FVector::Dist((*It)->GetActorLocation(), EndNode->GetActorLocation());
				if (!OpenSet.Contains(*It))
				{
					OpenSet.Add(*It);
				}
			}
		}
	}

	// If it exists this loop then no valid path has been found so return an empty path.

	return TArray<ANavigationNode*>();
}

TArray<ANavigationNode*> AAIManager::ReconstructPath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{
	TArray<ANavigationNode*> Path;
	ANavigationNode* CurrentNode = EndNode;
	while (CurrentNode != StartNode)
	{
		Path.Add(CurrentNode);
		CurrentNode = CurrentNode->CameFrom;
	}
	return Path;
}

void AAIManager::PopulateNodes()
{
	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		AllNodes.Add(*It);
		if (It->bIsMainNode)
		{
			MainNodes.Add(*It);
		}
		It->DrawDebug();
	}
}

void AAIManager::CreateAgents()
{
	if (AllNodes.Num() > 0)
	{
		for (int32 i = 0; i < NumAI; i++)
		{
			// Get a random node index
			CreateSingleAI();
		}

		for (int32 i = 0; i < NumAISquad; i++)
		{
			CreateSquad(FMath::RandRange(AISquadMinSize, AISquadMaxSize));
		}
	}
}

void AAIManager::CreateSingleAI()
{
	int32 NodeIndex = FMath::RandRange(0, AllNodes.Num() - 1);
	while (AllNodes[NodeIndex]->bIsOccupied)
	{
		NodeIndex = FMath::RandRange(0, AllNodes.Num() - 1);
	}
	AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(AgentToSpawn, AllNodes[NodeIndex]->GetActorLocation(), AllNodes[NodeIndex]->GetActorRotation());
	SpawnedEnemy->Manager = this;
	SpawnedEnemy->CurrentNode = AllNodes[NodeIndex];
	AllNodes[NodeIndex]->bIsOccupied = true;
}

void AAIManager::CreateSquad(int32 SquadMemberCount)
{

	if (MainNodes.Num() > 0)
	{
		int32 NodeIndex = FMath::RandRange(0, MainNodes.Num() - 1);
		int32 n = 5;
		while (MainNodes[NodeIndex]->bIsOccupied && n > 0)
		{
			NodeIndex = FMath::RandRange(0, MainNodes.Num() - 1);
			n--;
		}
		ANavigationNode* MainNode = MainNodes[NodeIndex];
		ANavigationNode* ChosenNode = MainNode;
		AEnemyCharacter* LastEnemy = nullptr;

		for (int i = 0; i < SquadMemberCount; i++)
		{
			n = 5;
			while (ChosenNode->bIsOccupied && n > 1)
			{
				ChosenNode = ChosenNode->ConnectedNodes[FMath::RandRange(0, ChosenNode->ConnectedNodes.Num() - 1)];
				n--;
			}

			AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(AgentToSpawn, ChosenNode->GetActorLocation(), ChosenNode->GetActorRotation());
			SpawnedEnemy->Manager = this;
			SpawnedEnemy->CurrentNode = ChosenNode;
			SpawnedEnemy->Superior = LastEnemy;
			if (LastEnemy) LastEnemy->Inferior = SpawnedEnemy;
			ChosenNode->bIsOccupied = true;
			LastEnemy = SpawnedEnemy;
		}

	}
}

ANavigationNode* AAIManager::FindNearestNode(const FVector& Location)
{
	ANavigationNode* NearestNode = nullptr;
	float NearestDistance = TNumericLimits<float>::Max();
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : MainNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance < NearestDistance)
		{
			NearestDistance = CurrentNodeDistance;
			NearestNode = CurrentNode;
		}
	}
	//UE_LOG(LogTemp, Error, TEXT("Nearest Node: %s"), *NearestNode->GetName());
	return NearestNode;
}

ANavigationNode* AAIManager::FindFurthestNode(const FVector& Location)
{
	ANavigationNode* FurthestNode = nullptr;
	float FurthestDistance = -1;
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : MainNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance > FurthestDistance)
		{
			FurthestDistance = CurrentNodeDistance;
			FurthestNode = CurrentNode;
		}
	}

	//UE_LOG(LogTemp, Error, TEXT("Furthest Node: %s"), *FurthestNode->GetName())
		return FurthestNode;
}

void AAIManager::GenerateNodes(const TArray<FVector>& Vertices, int32 Width, int32 Height)
{
	// Destroy all the ANavigationNodes
	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		(*It)->Destroy();
	}
	AllNodes.Empty();

	// Go through all the vertices and place the nodes.
	for (int i = 0; i < Vertices.Num(); i++)
	{
		ANavigationNode* NewNode = GetWorld()->SpawnActor<ANavigationNode>(Vertices[i], FRotator::ZeroRotator, FActorSpawnParameters());
		AllNodes.Add(NewNode);
	}

	// Add the connections between the nodes excluding the edge of the map as they do not have all 8 connection directions.
	for (int Y = 0; Y < Height; Y++)
	{
		for (int X = 0; X < Width; X++)
		{
			// When does N occur? - When the Y is not at the top of the grid
			if (!(Y == Height - 1))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y + 1) * Width + X]);
			}

			// When does NE occur? - When the X or the Y is not on the right or top of the grid respectvely
			if (!(Y == Height - 1 || X == 0))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y + 1) * Width + X - 1]);
			}

			// When does E occur? - When the X is on the right of the grid.
			if (!(X == 0))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[Y * Width + X - 1]);
			}

			// When does SE occur? - When the X or Y is not on the right or bottom of the grid respectively.
			if (!(X == 0 || Y == 0))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y - 1) * Width + X - 1]);
			}

			// When does S occur? - When the Y is not on the bottom of the grid.
			if (!(Y == 0))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y - 1) * Width + X]);
			}

			// When does SW occur? - When the X or Y is not on the left or bottom of the grid respectively.
			if (!(X == Width - 1 || Y == 0))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y - 1) * Width + X + 1]);
			}

			// When does W occur? - When the X is not on the left of the grid.
			if (!(X == Width - 1))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[Y * Width + X + 1]);
			}

			// When does NW occur? - When the X or Y is not on the left or top of the grid.
			if (!(X == Width - 1 || Y == Height - 1))
			{
				AddConnection(AllNodes[Y * Width + X], AllNodes[(Y + 1) * Width + X + 1]);
			}
		}
	}

}

void AAIManager::AddConnection(ANavigationNode* FromNode, ANavigationNode* ToNode)
{
	FVector DirectionVector = ToNode->GetActorLocation() - FromNode->GetActorLocation();
	DirectionVector.Normalize();
	if (FMath::Abs(DirectionVector.Z) < AllowedAngle)
	{
		if (!FromNode->ConnectedNodes.Contains(ToNode))
			FromNode->ConnectedNodes.Add(ToNode);

		if (!ToNode->ConnectedNodes.Contains(FromNode))
			ToNode->ConnectedNodes.Add(FromNode);
	}
}



