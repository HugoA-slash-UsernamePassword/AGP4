// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavigationNode.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NavigationNodeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API UNavigationNodeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNavigationNodeComponent();
	TSubclassOf<ANavigationNode> NavNode;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
