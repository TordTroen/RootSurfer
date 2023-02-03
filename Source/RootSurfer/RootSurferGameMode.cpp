// Copyright Epic Games, Inc. All Rights Reserved.

#include "RootSurferGameMode.h"
#include "RootSurferCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARootSurferGameMode::ARootSurferGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
