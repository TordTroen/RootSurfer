// Copyright Epic Games, Inc. All Rights Reserved.

#include "RootSurferCharacter.h"
#include "RootSurferProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SurfMovementComponent.h"
#include "CableComponent.h"
#include "Components/SceneComponent.h"


//////////////////////////////////////////////////////////////////////////
// ARootSurferCharacter

ARootSurferCharacter::ARootSurferCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	m_CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Grapple"));
	m_CableComponent->SetVisibility(false);
	m_CableComponent->SetupAttachment(FirstPersonCameraComponent);
}

void ARootSurferCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	m_SpeedToFovRatio = GetMovementComponent()->GetMaxSpeed() / m_MaxFov;
}

void ARootSurferCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Fov changer
	double TargetFov = FMath::Abs(GetVelocity().Size()) / m_SpeedToFovRatio;
	double NewFov = FMath::Lerp(FirstPersonCameraComponent->FieldOfView, TargetFov, m_FovChangeSpeed);
	NewFov = FMath::Clamp(NewFov, m_MinFov, m_MaxFov);
	FirstPersonCameraComponent->SetFieldOfView(NewFov);

	// Grapple movement
	if (m_GrappleHit.IsValidBlockingHit())
	{
		FVector Direction = (m_GrappleHit.ImpactPoint - GetActorLocation());
		Direction.Normalize();
		const FVector GrappleForce = Direction * DeltaTime * m_GrappleForce;
		GetCharacterMovement()->AddForce(GrappleForce);

		UpdateGrapple();
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ARootSurferCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ARootSurferCharacter::OnPressCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ARootSurferCharacter::StopCrouching);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARootSurferCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARootSurferCharacter::Look);

		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ARootSurferCharacter::DoPrimaryAction);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ARootSurferCharacter::StopPrimaryAction);
	}
}

void ARootSurferCharacter::UpdateGrapple()
{
	m_CableComponent->EndLocation = m_CableComponent->GetComponentTransform().InverseTransformPosition(m_GrappleHit.ImpactPoint);
	m_CableComponent->CableLength = m_CableComponent->EndLocation.Length();
}

void ARootSurferCharacter::StopGrapple()
{
	m_GrappleHit.Reset();
	m_CableComponent->SetVisibility(false);
	FlushPersistentDebugLines(GetWorld());
}

void ARootSurferCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X * m_LateralMovementSpeedModifier);
	}
}

void ARootSurferCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARootSurferCharacter::OnPressCrouch()
{
	auto* CHarMove = GetCharacterMovement();
	if (USurfMovementComponent* SurfComp = Cast<USurfMovementComponent>(GetCharacterMovement()))
	{
		SurfComp->ToggleCrouch(true);
		Crouch();
	}
}

void ARootSurferCharacter::StopCrouching()
{
	if (USurfMovementComponent* SurfComp = Cast<USurfMovementComponent>(GetCharacterMovement()))
	{
		SurfComp->ToggleCrouch(false);
		UnCrouch();
	}
}

void ARootSurferCharacter::Jump()
{
	Super::Jump();
	UnCrouch();
}

void ARootSurferCharacter::DoPrimaryAction()
{
	FHitResult Hit;
	const FVector StartOffset = GetFirstPersonCameraComponent()->GetForwardVector() * m_TraceStartOffset;
	const FVector TraceStart = GetFirstPersonCameraComponent()->GetComponentLocation() + StartOffset;
	const FVector TraceEnd = GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * m_PrimaryActionRange;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic);
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 2.0f);
	if (bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 32.0f, 32, FColor::Green, true, 2.0f);

		UpdateGrapple();
		
		// Enable moving to grapple
		m_GrappleHit = Hit;
		m_CableComponent->SetVisibility(true);
	}
	else
	{
		StopGrapple();
	}
}

void ARootSurferCharacter::StopPrimaryAction()
{
	StopGrapple();
}

void ARootSurferCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ARootSurferCharacter::GetHasRifle()
{
	return bHasRifle;
}