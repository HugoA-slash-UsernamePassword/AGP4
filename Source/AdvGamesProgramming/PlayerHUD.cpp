// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

APlayerHUD::APlayerHUD() 
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDObject(TEXT("/Game/Widgets/PlayerHUDWidget"));
	PlayerHUDClass = PlayerHUDObject.Class;
	if (PlayerHUDClass != nullptr && CurrentPlayerHUDWidget != nullptr)
	{
		CurrentPlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		CurrentPlayerHUDWidget->AddToViewport();
	}
}