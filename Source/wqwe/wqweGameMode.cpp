// Copyright Epic Games, Inc. All Rights Reserved.

#include "wqweGameMode.h"
#include "wqweCharacter.h"
#include "UObject/ConstructorHelpers.h"

AwqweGameMode::AwqweGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/WeaponSystem/Player/BP_Player"));
	//if (PlayerPawnBPClass.Class != NULL)
	{
		//DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
