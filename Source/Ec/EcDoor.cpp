// Fill out your copyright notice in the Description page of Project Settings.


#include "EcDoor.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEcDoor::AEcDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DoorDebugMarker = CreateDefaultSubobject<UArrowComponent>(TEXT("Door Debug Marker"));
	SetRootComponent(DoorDebugMarker);

	DoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Door Trigger"));
	DoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	DoorTrigger->SetCollisionResponseToChannel(ECC_GameTraceChannel12, ECR_Block);
	DoorTrigger->SetupAttachment(GetRootComponent());

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEcDoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEcDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEcDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEcDoor, IsOpen);
}

void AEcDoor::OnInteractStart_Implementation(AActor* EventInstigator)
{
	IsOpen = !IsOpen;
	OnRep_IsOpen();
}

void AEcDoor::OnInteractEnd_Implementation(AActor* EventInstigator)
{
}

void AEcDoor::OnRep_IsOpen_Implementation()
{
}
