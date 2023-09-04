// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomEnum.h"
#include "AIController.h"
#include "CoverSystemPublicData.h"
#include "Perception/AIPerceptionComponent.h"
#include "MyCharacterAI_Controller.generated.h"

UCLASS()
class WQWE_API AMyCharacterAI_Controller : public AAIController {
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "AIPerception")
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleDefaultsOnly, Category = "AIPerception")
	TObjectPtr<class UAISenseConfig_Sight> AISense_Sight;
	UPROPERTY(VisibleDefaultsOnly, Category = "AIPeception")
	TObjectPtr<class UAISenseConfig_Hearing> AISense_Hearing;
	UPROPERTY(VisibleDefaultsOnly, Category = "AIPerception")
	TObjectPtr<class UAISenseConfig_Damage> AISense_Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBehaviorTree> BTAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBlackboardData> BBAsset;

public:
	AMyCharacterAI_Controller();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AMyCharacterAI_Squad> Squad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AMyCharacterAI> OwnerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AMyCharacter> EnemyActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AAICoverPoint> CoverManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FCover CoverPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Perception_Sight = false;

	UPROPERTY()
	FTimerHandle Timer_Amsal;
	UPROPERTY()
	FTimerHandle Timer_GunFire;
	UPROPERTY()
	FTimerHandle Timer_PlayerLook;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAI_Search_Point_Manager> Search_Point_Manager_Class;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AAI_Search_Point_Manager> Search_Point_Manager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	E_EnemyState Current_State = E_EnemyState::E_StateOff;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	E_EnemyCombatCoverType Current_Cover = E_EnemyCombatCoverType::Cover_None;
	FAIStimulus Current_Stimulus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	float Active_Range = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	float Sense_Current = 0.0f;
	float Sense_Alert_Max = 10.0f;
	float Sense_Search_Increase = 1.0f;
	float Sense_Search_Max = 100.0f;
	float Sense_Attack_Max = 5.0f;

	float Distance_Short = 100.0f;
	float Distance_Middle = 200.0f;
	float Distance_Long = 300.0f;

	float Walk_Speed = 600.0f;

public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	UFUNCTION()
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus);

	UFUNCTION(BlueprintCallable)
	void StateChange(E_EnemyState NewState);

	UFUNCTION(BlueprintImplementableEvent)
	void Widget_Call();

	UFUNCTION(BlueprintImplementableEvent)
	void ReMoveWidget();

	void StartPatrol();
	void StartAlert(FVector vec);
	void StartSearch();
	void StartAttack();

	void SetSearchPointManager(AAI_Search_Point_Manager* Manager);
	void RunSearchPointManager(FVector Point);

	E_EnemyState GetCurrentState() const;
	void SetCurrentState(E_EnemyState NewState);
	
	E_EnemyCombatCoverType GetCurrentCover() const;
	void SetCurrentCover(E_EnemyCombatCoverType NewCover);
	
	float GetSenseCurrent() const;
	void SetSenseCurrent(float Input);

	TTuple<float, float> GetSightConfig();

	UFUNCTION(BlueprintPure)
	bool GetTarget();

	UFUNCTION()
	void GetAmsal(APawn* InPawn, FVector AILoc);
	UFUNCTION()
	void FinishAmsal();

	UFUNCTION()
	void LookPlayer();
	
	UFUNCTION()
	void StartWeaponFire();
	UFUNCTION()
	void StopWeaponFire();

public:
	static const FName BB_SelfActor;
	static const FName BB_EnemyActor;
	static const FName BB_CurrentState;
	static const FName BB_CurrentCombat;
	static const FName BB_CurrentCover;
	static const FName BB_NextVector;
	static const FName BB_NextRotator;
	static const FName BB_StartVector;
	static const FName BB_StartRotator;

	FORCEINLINE class UAIPerceptionComponent* GetAIPerception() const { return AIPerception; }
};
