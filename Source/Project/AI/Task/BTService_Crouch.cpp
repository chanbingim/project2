// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTService_Crouch.h"
#include "AIController.h"
#include "MyCharacterAI.h"

UBTService_DoCrouch::UBTService_DoCrouch() {
	NodeName = "Do Crouch";
}

void UBTService_DoCrouch::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<AMyCharacterAI> Pawn = Cast<AMyCharacterAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (Pawn == nullptr) {
		return;
	}
	Pawn->Crouch();

	if (bFuncCall == false) {
		bFuncCall = true;
		Pawn->UnCrouch();
	}
}
