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
	void ToggleCrouch(const bool bCrouch);
};
