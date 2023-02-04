// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfMovementComponent.h"
#include "RootSurferCharacter.h"

void USurfMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	m_InitialFriction = GroundFriction;
}

void USurfMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (m_bIsCrouching)
	{
		// TODO Only do this when we are on a slope. The force can depend on the angle of the slope?
		const FVector SlideForce = GetOwner()->GetActorForwardVector() * m_SlideForce * DeltaSeconds;
		AddForce(SlideForce);
	}
}

void USurfMovementComponent::ToggleCrouch(const bool bCrouch)
{
	m_bIsCrouching = bCrouch;
	GroundFriction = bCrouch ? m_SlideFriction : m_InitialFriction;
}

void USurfMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice /*= 0.f*/, const FVector& MoveDelta /*= FVector::ZeroVector*/)
{
	/*
	* Wall run:
	* - Start: Hit wall while in the air
	* - Stop: No more contact with wall, or contact with ground
	*		Can stop when velocity drops too much, or when jumping?
	*/
	UE_LOG(LogTemp, Display, TEXT("Handle impact %s"), *Hit.ToString());
	UE_LOG(LogTemp, Display, TEXT("Floor: %s"), *CurrentFloor.HitResult.ToString());
	if (IsFalling())
	{
		//SetMovementMode(MOVE_Custom, 0);
		//DoJump(false);
		//m_bIsWallRunning = true;
		//ARootSurferCharacter* Character = Cast<ARootSurferCharacter>(GetOwner());
		//Character->JumpCurrentCount --;
		//Character->Jump();
	}
}
