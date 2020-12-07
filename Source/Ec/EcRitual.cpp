// Fill out your copyright notice in the Description page of Project Settings.


#include "EcRitual.h"

#include "EcCharacter.h"
#include "EcGameMode.h"
#include "Components/BoxComponent.h"

// Sets default values
AEcRitual::AEcRitual()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RitualTableSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RitualTableSMesh"));
	SetRootComponent(RitualTableSMesh);

	RitualEnterZone = CreateDefaultSubobject<UBoxComponent>(TEXT("RitualEnterZone"));
	RitualEnterZone->SetupAttachment(GetRootComponent());

	// @Mouse, enables collision for both Querying (Raycasts etc..) and physics (for collision, or in this case overlaps)
	// Physics does collision in channels, so here we first blanket ignore collision on all channels and only re-enable
	// it for Channel Pawn (Pawns are characters controlled by a player or AI controller)
	RitualEnterZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RitualEnterZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	RitualEnterZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RitualEnterZone->OnComponentBeginOverlap.AddDynamic(this, &AEcRitual::OnRitualZoneEnter);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEcRitual::BeginPlay()
{
	Super::BeginPlay();
	
}

// @Mouse, This runs on both clients (on network relevant clients, so in this case all of them) and server 
void AEcRitual::OnRitualZoneEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto GM = Cast<AEcGameMode>(GetWorld()->GetAuthGameMode());

	// @Mouse
	// On clients get auth game mode returns a nullptr, so this check does two things
	// 1. Implicitly checks if the role of where this is being processed is the Authority (server), since the stuff
	// in this if only executes if GM isn't null
	// 2. Checks if the game mode is of correct type (the cast)
	if (GM) {
		const auto player = Cast<AEcCharacter>(OtherActor);

		if(player != nullptr && player->HasRelic())
		{
			UE_LOG(LogTemp, Log, TEXT("Player %s delivered relic"), *player->GetFullName());
			this->Relics.Add(player->DropOffRelic());
			GM->DeliverRelic(player);
		}
	}
}

// Called every frame
void AEcRitual::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

