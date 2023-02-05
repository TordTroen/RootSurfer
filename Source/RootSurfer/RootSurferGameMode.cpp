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
		ARootSurferCharacter* Character = Cast<ARootSurferCharacter>(m_TrackingPlayer->GetCharacter());
		//m_CurrentScore += m_ScorePerSecond * DeltaTime;
		const float Speed = Character->GetVelocity().Length();
		m_CurrentScore += Speed * m_ScoreSpeedModifier * DeltaTime;
	}
}
