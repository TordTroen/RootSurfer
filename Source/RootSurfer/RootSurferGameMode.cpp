// Copyright Epic Games, Inc. All Rights Reserved.

#include "RootSurferGameMode.h"
#include "RootSurferCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ARootSurferGameMode::ARootSurferGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_SurfPlayerCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PrimaryActorTick.bCanEverTick = true;

}

void ARootSurferGameMode::StartPlay()
{
	Super::StartPlay();
	m_TrackingPlayer = UGameplayStatics::GetPlayerController(this, 0);
}

void ARootSurferGameMode::Tick(float DeltaTime)
{
	if (m_TrackingPlayer != nullptr)
	{
		//* Character = Cast<ARootSurferCharacter>(m_TrackingPlayer->GetCharacter()))
		//if (m_TrackingChar == nullptr)
		//{
		//	m_TrackingChar = m_TrackingChar->GetChara
		//}
		//m_CurrentScore += m_ScorePerSecond * DeltaTime;
		if (ACharacter* Character = m_TrackingPlayer->GetCharacter())
		{
			const float Speed = Character->GetVelocity().Length();
			m_CurrentScore += Speed * m_ScoreSpeedModifier * DeltaTime;
		}
	}
}
