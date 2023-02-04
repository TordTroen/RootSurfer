// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfMovementComponent.h"

void USurfMovementComponent::ToggleCrouch(const bool bCrouch)
{
	GroundFriction = bCrouch ? 0.01f : 8.0f;
}
