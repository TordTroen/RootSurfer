// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RootSurferGameMode.generated.h"

UCLASS(minimalapi)
class ARootSurferGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARootSurferGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	float m_ScorePerSecond = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	float m_ScoreSpeedModifier = 1.0f;


	UPROPERTY(BlueprintReadWrite, Category = "Surf")
	float m_CurrentScore = 0.0f;

	APlayerController* m_TrackingPlayer = nullptr;
	ACharacter* m_TrackingChar = nullptr;
};



