// Fill out your copyright notice in the Description page of Project Settings.

#include "Character_PlayerController.h"

ACharacter_PlayerController::ACharacter_PlayerController() {
	PrimaryActorTick.bCanEverTick = true;
}

void ACharacter_PlayerController::BeginPlay() {
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
		FString::Printf(TEXT("PlayerController Team %d"), TeamId.GetId()));
}

FGenericTeamId ACharacter_PlayerController::GetGenericTeamId() const {
	//return IGenericTeamAgentInterface::GetGenericTeamId();
	return TeamId;
}

void ACharacter_PlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (team == false) {
		TeamId = FGenericTeamId(1);
	}
	else {
		TeamId = FGenericTeamId(5);
	}
}
