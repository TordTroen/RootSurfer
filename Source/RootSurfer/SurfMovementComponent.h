// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SurfMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ROOTSURFER_API USurfMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	void ToggleCrouch(const bool bCrouch);
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_SlideFriction = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_SlideForce = 50.0f;

	double m_InitialFriction = 8.0f;
	bool m_bIsCrouching = false;
};
