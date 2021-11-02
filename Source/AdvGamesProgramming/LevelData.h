// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavigationNode.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelData.generated.h"

USTRUCT(BlueprintType)
struct FMapNode
{
	GENERATED_BODY()
		// Use UPROPERTY() to decorate member variables as they allow for easier integration with network replication as well as potential garbage collection processing
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 start;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 end;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool main = false;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool oneway = false;

};

UCLASS()
class ADVGAMESPROGRAMMING_API ALevelData : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALevelData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapNode> connections;
	//TArray<int32> connections;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
