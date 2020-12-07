// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcRelicType.h"
#include "EcRelic.generated.h"

class AEcCharacter;
class UBoxComponent;

UCLASS()
class EC_API AEcRelic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEcRelic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRelicZoneEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EcRelicType Type;

	UPROPERTY(EditDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* RelicSMesh;

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	UBoxComponent* RelicEnterZone;
};
