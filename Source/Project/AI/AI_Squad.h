// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "AI_Squad.generated.h"

UCLASS()
class PROJECT_API AAI_Squad : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI_Squad();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	TArray<class ACharacter_AIController*> Squad_Array;

	virtual void Tick(float DeltaSeconds) override;
	
	void NotifyFindEnemy(class ACharacter_AI* Pawn, float Radius, class ACharacter_Base* EnemyActor);
	void NotifyEnemyIsDead(class ACharacter_AI* Pawn, float Radius);
	void NotifyFoundAllyDeadbody(class ACharacter_AI* Pawn, float Radius, FVector Center);
	void NotifyLostEnemy(class ACharacter_AI* Pawn, float Radius, FVector Center);

protected:
	TArray<AActor*> SearchNearbyAllies(FVector Center, float radius, class ACharacter_AI* Instigator);
};
