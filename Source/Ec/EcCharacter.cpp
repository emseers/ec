// Copyright Epic Games, Inc. All Rights Reserved.

#include "EcCharacter.h"

#include "EcInteractive.h"
#include "EcProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AEcCharacter

AEcCharacter::AEcCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create third person mesh, for visibility in MP
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->SetRelativeRotation(FRotator(0, -90.0f, 0));
	Mesh3P->SetRelativeLocation(FVector(0, 0, -100.0f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AEcCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEcCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AEcCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AEcCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEcCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AEcCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AEcCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AEcCharacter::OnInteractStart);
}

void AEcCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AEcProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AEcCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AEcCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AEcCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEcCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEcCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEcCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AEcCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AEcCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AEcCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AEcCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AEcCharacter::OnInteractStart()
{
	OnServerInteractStart();
}

void AEcCharacter::OnInteractStop()
{
	OnServerInteractStop();
}

void AEcCharacter::OnServerInteractStart_Implementation()
{
	FHitResult HitResult;
	const bool bHit = DoInteractionTrace(HitResult);

	if (bHit)
	{
		if (HitResult.GetComponent()->GetClass()->ImplementsInterface(UEcInteractive::StaticClass()))
		{
			IEcInteractive::Execute_OnInteractStart(HitResult.GetComponent(), this);
		} else if (HitResult.GetActor()->GetClass()->ImplementsInterface(UEcInteractive::StaticClass()))
		{
			IEcInteractive::Execute_OnInteractStart(HitResult.GetActor(), this);
		}
	}
}

bool AEcCharacter::OnServerInteractStart_Validate()
{
	return true;
}

void AEcCharacter::OnServerInteractStop_Implementation()
{
	if (CurrentInteractionTarget != nullptr)
	{
		if (CurrentInteractionTarget->GetClass()->ImplementsInterface(UEcInteractive::StaticClass()))
		{
			IEcInteractive::Execute_OnInteractEnd(CurrentInteractionTarget, this);
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Current interaction target somehow doesn't implement EcInteractive!"));
		}
	}
}

bool AEcCharacter::OnServerInteractStop_Validate()
{
	return true;
}

bool AEcCharacter::DoInteractionTrace(FHitResult& HitResult) const
{
	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const FCollisionQueryParams TraceParams(FName(TEXT("PlayerInteractTrace")), true, this);
	const FVector DistanceMax = CameraLocation + CameraRotation.Vector() * InteractionDistance;
	GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, DistanceMax, ECC_GameTraceChannel2, TraceParams);

	if (HitResult.GetComponent() && HitResult.IsValidBlockingHit())
	{
		return true;
	}

	return false;
}

bool AEcCharacter::HasRelic()
{
	return this->RelicType != EcRelicType::None;
}

bool AEcCharacter::PickupRelic(const EcRelicType& relicType)
{
	if (this->HasRelic()) {
		return false;
	}

	this->RelicType = relicType;
	return true;
}

EcRelicType AEcCharacter::DropOffRelic()
{
	if (!this->HasRelic()) {
		UE_LOG(LogTemp, Warning, TEXT("Player %s doesn't have a relic to drop off!"), *this->GetFullName());
		return EcRelicType::None;
	}

	EcRelicType relicType = this->RelicType;
	this->RelicType = EcRelicType::None;
	return relicType;
}

void AEcCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEcCharacter, RelicType);
}
