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

	m_SpeedLevels.Add(1000.0f);
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
	const double CurCharSpeed = GetVelocity().Size();
	double TargetFov = FMath::Abs(CurCharSpeed) / m_SpeedToFovRatio;
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

	// Speed levels
	int32 NewSpeedLevel = 0;
	if (CurCharSpeed <= SMALL_NUMBER)
	{
		NewSpeedLevel = 0;
	}
	else
	{
		for (NewSpeedLevel = 0; NewSpeedLevel < m_SpeedLevels.Num(); NewSpeedLevel++)
		{
			if (CurCharSpeed < m_SpeedLevels[NewSpeedLevel])
			{
				break;
			}
		}
	}
	if (NewSpeedLevel != m_SpeedLevel)
	{
		m_SpeedLevel = NewSpeedLevel;
		OnSpeedLevelChange(m_SpeedLevel);
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

void ARootSurferCharacter::AttachGrapple(FHitResult Hit)
{
	UpdateGrapple();
	m_GrappleHit = Hit;
	m_CableComponent->SetVisibility(true);
	OnGrappleHit(m_GrappleHit.ImpactPoint);
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
		OnBeginSlide();
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
	OnJump();
}

void ARootSurferCharacter::DoPrimaryAction()
{
	FHitResult Hit;
	const FVector StartOffset = GetFirstPersonCameraComponent()->GetForwardVector() * m_TraceStartOffset;
	const FVector TraceStart = GetFirstPersonCameraComponent()->GetComponentLocation() + StartOffset;
	const FVector TraceEnd = GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * m_PrimaryActionRange;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, true, 2.0f);
	if (bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 32.0f, 32, FColor::Green, true, 2.0f);

		AttachGrapple(Hit);
	}
	else
	{
		const float Radius = 100.0f;
		
		//static const FName GrappleSphereTrace(TEXT("Grapple_ForgivingTrace"));
		//FCollisionQueryParams Params(GrappleSphereTrace, false, GetOwner());
		//Params.AddIgnoredActor(GetOwner());
		//const bool bForgivingBlockingHit = GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, FCollisionShape::MakeSphere(Radius), Params);
		//if (bForgivingBlockingHit)
		//{
		//	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 32.0f, 32, FColor::Red, true, 2.0f);

		//	AttachGrapple(Hit);
		//}
		//else
		{
			//StopGrapple();
		}
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