// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BinauralGameMode.h"
#include "BinauralHUD.h"
#include "BinauralCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABinauralGameMode::ABinauralGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABinauralHUD::StaticClass();
}
