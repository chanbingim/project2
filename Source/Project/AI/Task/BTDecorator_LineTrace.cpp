// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_LineTrace.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Camera/CameraComponent.h"
#include "MyCharacter.h"
#include "MyCharacterAI.h"
#include "WeaponComponent.h"


UBTDecorator_LineTrace::UBTDecorator_LineTrace(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeName = "LineTrace Deco";
	bCreateNodeInstance = true;

	BlackboardKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());
	View_Point = FVector::ZeroVector;
	Check = false;
}

bool UBTDecorator_LineTrace::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	const TObjectPtr<UBlackboardComponent> BB = OwnerComp.GetBlackboardComponent();
	const TObjectPtr<AMyCharacterAI_Controller> Controller = Cast<AMyCharacterAI_Controller>(OwnerComp.GetAIOwner());
	const TObjectPtr<AMyCharacterAI> Pawn = Cast<AMyCharacterAI>(Controller->GetPawn());
	if ((BB == nullptr) || (Controller == nullptr) || (Pawn == nullptr)) {
		return false;
	}

	// FHitResult RayHit;
	// FVector PreStart = Pawn->GetWeaponComponent()->AimPointCamera->GetComponentLocation();
	// FVector PreEnd = PreStart + Pawn->GetWeaponComponent()->AimPointCamera->GetForwardVector() * 10000.0f;
	// GetWorld()->LineTraceSingleByChannel(RayHit, PreStart, PreEnd, ECollisionChannel::ECC_GameTraceChannel2);
	// if (IsValid(Cast<AMyCharacter>(RayHit.GetActor())) == false) {
	// 	return false;
	// }

	auto KeyName = BB->GetKeyID(BlackboardKey.SelectedKeyName);
	UObject* KeyValue = BB->GetValue<UBlackboardKeyType_Object>(KeyName);
	AActor* TargetActor = Cast<AActor>(KeyValue);
	if (IsValid(TargetActor) == false) {
		return false;
	}
	return Controller->LineOfSightTo(TargetActor, View_Point, Check);
}
