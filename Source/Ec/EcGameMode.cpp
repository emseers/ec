// Copyright Epic Games, Inc. All Rights Reserved.

#include "EcGameMode.h"
#include "EcHUD.h"
#include "EcCharacter.h"
#include "EcGameState.h"
#include "UObject/ConstructorHelpers.h"

AEcGameMode::AEcGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AEcHUD::StaticClass();
	GameStateClass = AEcGameState::StaticClass();
}

void AEcGameMode::DeliverRelic(AEcCharacter* deliveryPlayer)
{
	AEcGameState* GS = GetGameState<AEcGameState>();

	GS->CurrentRelics = GS->CurrentRelics + 1;
	UE_LOG(LogTemp, Log, TEXT("%d out of %d relics collected"), GS->CurrentRelics, GS->RelicsToWin);
	
	if (GS->CurrentRelics >= GS->RelicsToWin)
	{
		UE_LOG(LogTemp, Log, TEXT("All relics collected, game won"));
	}
}
