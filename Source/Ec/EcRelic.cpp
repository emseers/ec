// Fill out your copyright notice in the Description page of Project Settings.


#include "EcRelic.h"

#include "EcCharacter.h"
#include "EcGameMode.h"
#include "Components/BoxComponent.h"

// Sets default values
AEcRelic::AEcRelic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RelicSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RitualTableSMesh"));
	SetRootComponent(RelicSMesh);

	RelicEnterZone = CreateDefaultSubobject<UBoxComponent>(TEXT("RelicEnterZone"));
	RelicEnterZone->SetupAttachment(GetRootComponent());

	RelicEnterZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RelicEnterZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	RelicEnterZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RelicEnterZone->OnComponentBeginOverlap.AddDynamic(this, &AEcRelic::OnRelicZoneEnter);
}

// Called when the game starts or when spawned
void AEcRelic::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEcRelic::OnRelicZoneEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto GM = Cast<AEcGameMode>(GetWorld()->GetAuthGameMode());

	if (GM) {
		const auto player = Cast<AEcCharacter>(OtherActor);

		if (player && player->PickupRelic(this->Type))
		{
			UE_LOG(LogTemp, Log, TEXT("Player %s acquired relic"), *player->GetFullName());
			this->Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Player %s already has a relic"), *player->GetFullName());
		}
	}
}

// Called every frame
void AEcRelic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

