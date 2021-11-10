// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Engine/GameEngine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerHUD.h"
#include "PlayerCharacter.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxHealth = 100.0f;
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::OnTakeDamage(float Damage)
{
	CurrentHealth -= Damage;
	UpdateHealthBar();
	if (CurrentHealth < 0.0f)
	{
		CurrentHealth = 0;
		OnDeath();
		CurrentHealth = MaxHealth;
	}
	
}



void UHealthComponent::OnDeath()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnDeath();
	}
}

float UHealthComponent::HealthPercentageRemaining()
{
	return CurrentHealth/MaxHealth * 100.0f;
}

void UHealthComponent::UpdateHealthBar()
{
	//if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		if (PlayerHUD)
		{
			PlayerHUD->SetPlayerHealthBarPercent(CurrentHealth / MaxHealth);
		}
	}
}
