// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcRelicType.h"
#include "EcRitual.generated.h"

class AEcCharacter;
class UBoxComponent;

UCLASS()
class EC_API AEcRitual : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AEcRitual();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRitualZoneEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY()
	TArray<EcRelicType> Relics;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* RitualTableSMesh;

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	UBoxComponent* RitualEnterZone;
};
