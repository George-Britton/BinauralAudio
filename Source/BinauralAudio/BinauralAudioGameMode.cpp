// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BinauralAudioGameMode.h"
#include "BinauralAudioHUD.h"
#include "BinauralAudioCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABinauralAudioGameMode::ABinauralAudioGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABinauralAudioHUD::StaticClass();
}
