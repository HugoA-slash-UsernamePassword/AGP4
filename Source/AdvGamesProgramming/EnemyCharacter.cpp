// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AIManager.h"
#include "NavigationNode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentAgentState = AgentState::PATROL;
	PathfindingDestinationAccuracy = 100.0f;
	PathfindingNodeAccuracy = PathfindingDestinationAccuracy;

	Superior = nullptr;
	Inferior = nullptr;
}

AEnemyCharacter::AEnemyCharacter(AEnemyCharacter* SuperiorAI)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentAgentState = AgentState::PATROL;
	PathfindingDestinationAccuracy = 100.0f;
	PathfindingNodeAccuracy = PathfindingDestinationAccuracy;

	Superior = SuperiorAI;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	Cast<UCharacterMovementComponent>(GetMovementComponent())->bOrientRotationToMovement = true;

	HealthComponent = FindComponentByClass<UHealthComponent>();

	PerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::SensePlayer);
	}

	DetectedActor = nullptr;
	bCanSeeActor = false;
	TravelTimer = MaxTimeBeforeReroute;

	PathfindingNodeAccuracy = PathfindingDestinationAccuracy;

}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Superior && HealthComponent->CurrentHealth > 40.0f)
	{
		if (bCanSeeActor && !Superior->bCanSeeActor)
		{
			Superior->DetectedActor = DetectedActor;
			Superior->Path.Empty();
			Superior->AgentEngage();
			Superior->CurrentAgentState = AgentState::ENGAGE;

			Path.Empty();
			AgentEngage();

			UE_LOG(LogTemp, Warning, TEXT("PLAYER SPOTTED"));
		}

		CurrentAgentState = Superior->CurrentAgentState;
		DetectedActor = Superior->DetectedActor;
	}

	FindPath();


	if (HealthComponent->CurrentHealth == 0)
	{
		if (Inferior)Inferior->Superior = Superior;
		if (Superior)Superior->Inferior = Inferior;
		Destroy();
	}

	if (TravelTimer > 0) TravelTimer -= DeltaTime;

	if (LastNode)
	{
		if ((GetActorLocation() - LastNode->GetActorLocation()).SizeSquared() > 100)
		{
			LastNode->bIsOccupied = false;
		}
	}

	MoveAlongPath();
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol()
{
	if (Path.Num() == 0)
	{
		if (Superior && Manager)
		{
			Path = Manager->GeneratePath(CurrentNode, Superior->CurrentNode);
		}
		else if (Manager)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->MainNodes[FMath::RandRange(0, Manager->MainNodes.Num() - 1)]);
		}
	}
}

void AEnemyCharacter::AgentEngage()
{
	if (bCanSeeActor && DetectedActor)
	{
		FVector FireDirection = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(FireDirection);
	}
	if (Path.Num() == 0 && DetectedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENGAGING PLAYER"));

		ANavigationNode* NearestNode = Manager->FindNearestNode(DetectedActor->GetActorLocation());
		if (NearestNode != CurrentNode && !(CurrentNode->ConnectedNodes.Contains(NearestNode) && NearestNode->bIsOccupied))
		{
			Path = Manager->GeneratePath(CurrentNode, NearestNode);
		}

	}
}

void AEnemyCharacter::AgentEvade()
{
	if (bCanSeeActor && DetectedActor)
	{
		FVector FireDirection = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(FireDirection);
	}
	if (Path.Num() == 0 && DetectedActor)
	{

		ANavigationNode* FurthestNode = Manager->FindFurthestNode(DetectedActor->GetActorLocation());

		if (FurthestNode != CurrentNode && !(CurrentNode->ConnectedNodes.Contains(FurthestNode) && FurthestNode->bIsOccupied))
		{
			Path = Manager->GeneratePath(CurrentNode, FurthestNode);
		}
	}
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus)
{
	if (ActorSensed->ActorHasTag(FName(TEXT("Player"))))
	{
		if (Stimulus.WasSuccessfullySensed())
		{

			UE_LOG(LogTemp, Warning, TEXT("Player Detected"))
				DetectedActor = ActorSensed;
			bCanSeeActor = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
				bCanSeeActor = false;
		}
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (!(GetLocalRole() == ENetRole::ROLE_Authority)) return;
	if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy)
		&& Path.Num() > 0)
	{
		TravelTimer = MaxTimeBeforeReroute;
		LastNode = CurrentNode;
		CurrentNode = Path.Pop();
		ANavigationNode* NextNode = CurrentNode;
		bIsStuck = false;

		if (Path.Num() == 0 && CurrentNode->bIsOccupied)
		{
			int8 i = 5;
			while (NextNode->bIsOccupied && i >= 5)
			{
				NextNode = NextNode->ConnectedNodes[FMath::RandRange(0, NextNode->ConnectedNodes.Num() - 1)];
				i--;
			}

			Path.Empty();
			Path = Manager->GeneratePath(LastNode, NextNode);
			CurrentNode = LastNode;
			PathfindingNodeAccuracy = PathfindingEvasionAccuracy;

			UE_LOG(LogTemp, Error, TEXT("%s Node: %s is Occupied. Moving to %s"), *GetName(), *CurrentNode->GetName(), *NextNode->GetName());

		}

		if (Path.Num() == 0)
		{
			CurrentNode->bIsOccupied = true;
			UE_LOG(LogTemp, Error, TEXT("%s Node: %s has been Occupied"), *GetName(), *CurrentNode->GetName());
			PathfindingNodeAccuracy = PathfindingDestinationAccuracy;
		}

	}
	else if (!(GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy))
	{
		AddMovementInput(CurrentNode->GetActorLocation() - GetActorLocation());
		if (TravelTimer <= 0)
		{
			TravelTimer = MaxTimeBeforeReroute;
			Path.Insert(CurrentNode, 0);
			if (!LastNode)
			{
				CurrentNode = Manager->FindNearestNode(GetActorLocation());
				FindPath();

			}
			else if (LastNode->bIsOccupied)
			{
				CurrentNode = Manager->FindNearestNode(GetActorLocation());
				FindPath();
			}
			else if (bIsStuck)
			{
				CurrentNode = Manager->FindNearestNode(GetActorLocation());
				FindPath();
			}
			else
			{
				CurrentNode = LastNode;
				FindPath();
			}

			bIsStuck = true;

		}
	}

	/*if (Path.Num() == 1 && CurrentNode->bIsOccupied)
	{
		Path.Add(CurrentNode->ConnectedNodes[FMath::RandRange(0, CurrentNode->ConnectedNodes.Num() - 1)]);
		CurrentNode = Path.Pop();
	}*/
}

void  AEnemyCharacter::FindPath()
{
	if (!(GetLocalRole() == ENetRole::ROLE_Authority)) return;
	if (CurrentAgentState == AgentState::PATROL)
	{
		AgentPatrol();
		if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 40.0f)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 40.0f)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::ENGAGE)
	{
		AgentEngage();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 40.0f)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::EVADE)
	{
		AgentEvade();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 40.0f)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
	}
}

