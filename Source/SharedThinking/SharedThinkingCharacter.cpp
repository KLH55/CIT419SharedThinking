// Copyright Epic Games, Inc. All Rights Reserved.

#include "SharedThinkingCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Net/UnrealNetwork.h"
#include "SharedThinking.h"
#include "GameTimer.h"
#include "Kismet/GameplayStatics.h"

ASharedThinkingCharacter::ASharedThinkingCharacter()
{
	//Player can use ticks
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// Physics for picking up and dropping items
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	HoldLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldLocation"));
	HoldLocationComponent->SetupAttachment(FirstPersonCameraComponent);
	HoldLocationComponent->SetRelativeLocation(FVector(130.0f, 0.0f, -20.0f));

	// Overhead Nameplate Widget Component
	NameplateWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidgetComp"));
	NameplateWidgetComp->SetupAttachment(RootComponent);
	NameplateWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));
	NameplateWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	NameplateWidgetComp->SetDrawAtDesiredSize(true);
	NameplateWidgetComp->SetOwnerNoSee(true);

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ASharedThinkingCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASharedThinkingCharacter, ReplicatedPlayerName);
}

void ASharedThinkingCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure late-joining clients display existing player names immediately
	UpdateNameplateUI();
}

// Tracking items location
void ASharedThinkingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((HasAuthority() || IsLocallyControlled()) && PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FRotator AimRotation = GetControlRotation();
		FVector HandOffset = FVector(130.0f, 0.0f, -20.0f);

		FVector BaseEyeLocation;
		FRotator BaseEyeRotation;
		GetActorEyesViewPoint(BaseEyeLocation, BaseEyeRotation);

		FVector TargetLocation = BaseEyeLocation + AimRotation.RotateVector(HandOffset);
		FRotator TargetRotation = AimRotation;

		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, TargetRotation);
	}
}

void ASharedThinkingCharacter::SubmitPlayerName(const FString& NewName)
{
	Server_SetPlayerName(NewName);
}

void ASharedThinkingCharacter::Server_SetPlayerName_Implementation(const FString& NewName)
{
	FString SanitizedName = NewName.TrimStartAndEnd().Left(20);
	if (SanitizedName.IsEmpty())
	{
		SanitizedName = TEXT("Player");
	}

	ReplicatedPlayerName = SanitizedName;

	// Find the AGameTimer actor in the level and register this player's name
	if (AGameTimer* TimerActor = Cast<AGameTimer>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameTimer::StaticClass())))
	{
		TimerActor->RegisterPlayerName(GetController(), SanitizedName);
	}

	// Manually trigger OnRep so the Listen Server host also updates its UI
	OnRep_PlayerName();
}

void ASharedThinkingCharacter::OnRep_PlayerName()
{
	UpdateNameplateUI();
}

void ASharedThinkingCharacter::UpdateNameplateUI()
{
	if (!NameplateWidgetComp) return;

	if (UUserWidget* WidgetInstance = NameplateWidgetComp->GetUserWidgetObject())
	{
		if (UTextBlock* NameText = Cast<UTextBlock>(WidgetInstance->GetWidgetFromName(TEXT("PlayerNameText"))))
		{
			NameText->SetText(FText::FromString(ReplicatedPlayerName));
		}
	}
}

void ASharedThinkingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASharedThinkingCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASharedThinkingCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASharedThinkingCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASharedThinkingCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASharedThinkingCharacter::LookInput);

		// Pickup/Drop
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASharedThinkingCharacter::InteractInput);
	}
	else
	{
		UE_LOG(LogSharedThinking, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASharedThinkingCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASharedThinkingCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

// How the player interacts with items
void ASharedThinkingCharacter::InteractInput(const FInputActionValue& Value)
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		if (HasAuthority())
		{
			Drop();
		}
		else 
		{
			Server_Interact(FVector::ZeroVector, FVector::ZeroVector);
		}
		return;
	}

	if (FirstPersonCameraComponent)
	{
		FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector CameraForward = FirstPersonCameraComponent->GetForwardVector();

		Server_Interact(CameraLocation, CameraForward);
	}
}

bool ASharedThinkingCharacter::Server_Interact_Validate(FVector TraceStart, FVector TraceDirection)
{
	return true;
}

void ASharedThinkingCharacter::Server_Interact_Implementation(FVector TraceStart, FVector TraceDirection)
{
	// Drops if already carrying something
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		Drop();
		return;
	}
		
	FVector EndLocation = TraceStart + (TraceDirection * InteractTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, EndLocation, ECC_PhysicsBody, QueryParams);

	if (bHit && HitResult.GetComponent())
	{
		UPrimitiveComponent* TargetComponent = HitResult.GetComponent();

		if (TargetComponent->IsSimulatingPhysics())
		{
			Pickup(TargetComponent, HitResult.ImpactPoint, TargetComponent->GetComponentRotation());
		}
	}
}

// Picking up item in view
void ASharedThinkingCharacter::Pickup(UPrimitiveComponent* ComponentToPickUp, FVector HitLocation, FRotator HitRotation)
{
	if (!ComponentToPickUp) return;
	
	if (HasAuthority())
	{
		ComponentToPickUp->WakeAllRigidBodies();

		Multicast_Pickup(ComponentToPickUp, HitLocation, HitRotation);
	}

	if (!PhysicsHandle) return;

	PhysicsHandle->SetTargetRotation(HitRotation);
	PhysicsHandle->GrabComponentAtLocationWithRotation(
		ComponentToPickUp,
		NAME_None,
		HitLocation,
		HitRotation
	);
}

void ASharedThinkingCharacter::Multicast_Pickup_Implementation(UPrimitiveComponent* ComponentToPickUp, FVector HitLocation, FRotator HitRotation)
{
	if (!PhysicsHandle || !ComponentToPickUp) return;

	if (HasAuthority() || IsLocallyControlled())
	{
		PhysicsHandle->SetTargetRotation(HitRotation);
		PhysicsHandle->GrabComponentAtLocationWithRotation
		(
			ComponentToPickUp,
			NAME_None,
			HitLocation,
			HitRotation
		);
	}
}

// Dropping item being held
void ASharedThinkingCharacter::Drop()
{
	if (HasAuthority()) 
	{
		Multicast_Drop();
	}
}

void ASharedThinkingCharacter::Multicast_Drop_Implementation() 
{
	if (PhysicsHandle) 
	{
		PhysicsHandle->ReleaseComponent();
	}
}

void ASharedThinkingCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASharedThinkingCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ASharedThinkingCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ASharedThinkingCharacter::DoJumpEnd()
{
// pass StopJumping to the character
StopJumping();
}