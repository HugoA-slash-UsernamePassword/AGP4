// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationNode.h"
#include "DrawDebugHelpers.h"

// Sets default values
ANavigationNode::ANavigationNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));
	RootComponent = LocationComponent;
	bReplicates = true;
}

void ANavigationNode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	DOREPLIFETIME(ANavigationNode, bIsOccupied);
}

// Called when the game starts or when spawned
void ANavigationNode::BeginPlay()
{
	Super::BeginPlay();

	DrawDebug();
	
}

void ANavigationNode::DrawDebug()
{
	for (auto It = ConnectedNodes.CreateConstIterator(); It; It++)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), (*It)->GetActorLocation(), FColor::White, true,-1.0f, (uint8)'\000', 1.0f);
	}
}

// Called every frame
void ANavigationNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ANavigationNode::FScore()
{
	return GScore + HScore;
}

