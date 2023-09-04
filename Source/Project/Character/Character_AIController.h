// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CustomEnum.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character_AIController.generated.h"

class ACharacter_Player;
UCLASS()
class PROJECT_API ACharacter_AIController : public AAIController {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIPerception", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BTAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	class UBlackboardData* BBAsset;

public:
	ACharacter_AIController();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	class AAI_Squad* Squad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	class AAI_Search_Point_Manager* Search_Point_Manager;
	
private:
	UPROPERTY()
	class ACharacter_Base* StimulusCharacter;

	UPROPERTY()
	class ACharacter_AI* OwnerPawn;
	
	E_EnemyState CurrentState = E_EnemyState::E_StatePatrol;
	FAIStimulus CurrentStimulus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int32 AlertTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int32 AlertTimer_Max = 1200;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int32 SearchTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int32 SearchTimer_Max = 300;

	bool bPerception = false;

	UPROPERTY()
	class ACharacter_Player* Player;

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	UFUNCTION()
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION(BlueprintCallable, Category = "Test")
	void StateChange(E_EnemyState NewState);

	void StartAlert(FVector vec);
	void FinishAlert();
	
	
	

	/*
	static const FName BB_HomePosKey;
	static const FName BB_PatrolPosKey;
	static const FName BB_TargetObjKey;

	
	static const FName BB_SearchWaypointHolder;
	static const FName BB_SelfActor;
	static const FName BB_AttackRangedDistance;
	static const FName BB_Destination;
	static const FName BB_DesiredRotation;
	static const FName BB_CurrentState;
	static const FName BB_RandomCenterPoint;
	static const FName BB_RandomPatrolRange;
	*/

	static const FName BB_EnemyActor;
	static const FName BB_MysteryActor;

	static const FName BB_CurrentState;
	static const FName BB_NextVector;
	static const FName BB_NextRotator;
	
	FORCEINLINE class UAIPerceptionComponent* GetAIPerception() const { return AIPerception; }
};
