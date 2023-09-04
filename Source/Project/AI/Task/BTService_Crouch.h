// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_Crouch.generated.h"

UCLASS()
class PROJECT_API UBTService_DoCrouch : public UBTService_BlackboardBase {
	GENERATED_BODY()

	bool bFuncCall = false;
public:
	UBTService_DoCrouch();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
