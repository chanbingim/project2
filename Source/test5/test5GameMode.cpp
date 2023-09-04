// Copyright Epic Games, Inc. All Rights Reserved.

#include "test5GameMode.h"
#include "test5Character.h"
#include "UObject/ConstructorHelpers.h"

Atest5GameMode::Atest5GameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));

	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Script/Engine.Blueprint'/Game/WeaponSystem/Player/BP_Player2.BP_Player2'"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
}
