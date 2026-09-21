// Copyright Epic Games, Inc. All Rights Reserved.

#include "SharedThinkingCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "SharedThinking.h"

ASharedThinkingCharacter::ASharedThinkingCharacter()
{

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

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	HoldLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldLocation"));
	HoldLocationComponent->SetupAttachment(FirstPersonCameraComponent);
	HoldLocationComponent->SetRelativeLocation(FVector(130.0f, 0.0f, -20.0f));

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ASharedThinkingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = HoldLocationComponent->GetComponentLocation();
		FRotator TargetRotation = HoldLocationComponent->GetComponentRotation();

		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, TargetRotation);
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

void ASharedThinkingCharacter::InteractInput(const FInputActionValue& Value)
{
	// Drops if already carrying something
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		Drop();
		return;
	}

	// Line trace from camera to see objects.
	if (FirstPersonCameraComponent)
	{
		FVector StartLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector EndLocation = StartLocation + (FirstPersonCameraComponent->GetForwardVector() * InteractTraceDistance);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_PhysicsBody, QueryParams);

		if (bHit && HitResult.GetComponent())
		{
			UPrimitiveComponent* TargetComponent = HitResult.GetComponent();

			if (TargetComponent->IsSimulatingPhysics())
			{
				Pickup(TargetComponent, HitResult.ImpactPoint, TargetComponent->GetComponentRotation());
			}
		}
	}
}

void ASharedThinkingCharacter::Pickup(UPrimitiveComponent* ComponentToPickUp, FVector HitLocation, FRotator HitRotation)
{
	if (!PhysicsHandle) return;

	PhysicsHandle->SetTargetRotation(HitRotation);
	PhysicsHandle->GrabComponentAtLocationWithRotation(
		ComponentToPickUp,
		NAME_None,
		HitLocation,
		HitRotation
	);
}

void ASharedThinkingCharacter::Drop()
{
	if (!PhysicsHandle) return;
	PhysicsHandle->ReleaseComponent();
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
