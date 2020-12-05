// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EcGameState.generated.h"

/**
 * 
 */
UCLASS()
class EC_API AEcGameState : public AGameStateBase
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	int CurrentRelics = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	int RelicsToWin = 3;
};
