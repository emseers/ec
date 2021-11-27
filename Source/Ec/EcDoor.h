// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EcInteractive.h"
#include "GameFramework/Actor.h"
#include "EcDoor.generated.h"

class UArrowComponent;
class UBoxComponent;
UCLASS()
class EC_API AEcDoor : public AActor, public IEcInteractive
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* DoorTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* DoorDebugMarker;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, ReplicatedUsing="OnRep_IsOpen", Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	bool IsOpen = false;
public:
	// Sets default values for this actor's properties
	AEcDoor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnInteractStart_Implementation(AActor* EventInstigator) override;
	virtual void OnInteractEnd_Implementation(AActor* EventInstigator) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnRep_IsOpen();
};
