#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_LineTrace.generated.h"

UCLASS()
class PROJECT_API UBTDecorator_LineTrace : public UBTDecorator_BlackboardBase {
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UBTDecorator_LineTrace(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, Category = "Condition")
	FVector View_Point;
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool Check;
};
