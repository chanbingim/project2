// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter_Controller.h"

AMyCharacter_Controller::AMyCharacter_Controller() {
	PrimaryActorTick.bCanEverTick = true;
}

FGenericTeamId AMyCharacter_Controller::GetGenericTeamId() const {
	//return IGenericTeamAgentInterface::GetGenericTeamId();
	return TeamId;
}

void AMyCharacter_Controller::BeginPlay() {
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("PlayerController Team %d"), TeamId.GetId()));
}

void AMyCharacter_Controller::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	
	if (GetPawn()) {
		this->AttachToPawn(InPawn);
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%s PlayerOwner : %s"), *this->GetName(), *GetPawn()->GetName()));
	}
}

void AMyCharacter_Controller::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (team == false) {
		TeamId = FGenericTeamId(1);
	}
	else {
		TeamId = FGenericTeamId(5);
	}
}
