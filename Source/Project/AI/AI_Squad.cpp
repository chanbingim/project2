// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AI_Squad.h"

#include "Character_AIController.h"
#include "Character_AI.h"
#include "NavigationSystem.h"
#include "Kismet\KismetSystemLibrary.h"

// Sets default values
AAI_Squad::AAI_Squad() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAI_Squad::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void AAI_Squad::NotifyFindEnemy(ACharacter_AI* Pawn, float Radius, ACharacter_Base* EnemyActor) {
	TArray<AActor*> Allies = SearchNearbyAllies(Pawn->GetActorLocation(), Radius, Pawn);

	for (INT32 i = 0; i < Allies.Num(); i += 1)	{
	 	ACharacter_Base* tCharacter = Cast<ACharacter_Base>(Allies[i]);
		if (tCharacter != nullptr) {
			if (tCharacter->IsDead() == false) {
				ACharacter_AIController* AICtrl =  Cast<ACharacter_AIController>(tCharacter->GetController<AAIController>());
				if (AICtrl != nullptr) {
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("AICTRL %s"), *AICtrl->GetName()));
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyFindEnemy Cast<ACharacter_AI> Fail")));
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyFindEnemy Cast<ACharacter_Base> Fail")));
		}
	}
}

void AAI_Squad::NotifyEnemyIsDead(ACharacter_AI* Pawn, float Radius) {
	TArray<AActor*> Allies = SearchNearbyAllies(Pawn->GetActorLocation(), Radius, Pawn);

	for (INT32 i = 0; i < Allies.Num(); i += 1)	{
		ACharacter_Base* tCharacter = Cast<ACharacter_Base>(Allies[i]);
		if (tCharacter != nullptr) {
			if (tCharacter->IsDead() == false) {
				ACharacter_AIController* AICtrl =  Cast<ACharacter_AIController>(tCharacter->GetController<AAIController>());
				if (AICtrl != nullptr) {
					
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyEnemyIsDead Cast<ACharacter_AI> Fail")));
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyEnemyIsDead Cast<ACharacter_Base> Fail")));
		}
	}
}

void AAI_Squad::NotifyFoundAllyDeadbody(ACharacter_AI* Pawn, float Radius, FVector Center) {
	TArray<AActor*> Allies = SearchNearbyAllies(Pawn->GetActorLocation(), Radius, Pawn);

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) {
		return;
	}
	
	FNavLocation RandomLocation;
	NavSystem->GetRandomReachablePointInRadius(Center, 250.0f, RandomLocation);
	
	for (INT32 i = 0; i < Allies.Num(); i += 1)	{
		ACharacter_Base* tCharacter = Cast<ACharacter_Base>(Allies[i]);
		if (tCharacter != nullptr) {
			if (tCharacter->IsDead() == false) {
				ACharacter_AIController* AICtrl =  Cast<ACharacter_AIController>(tCharacter->GetController<AAIController>());
				if (AICtrl != nullptr) {

				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyFoundAllyDeadbody Cast<ACharacter_AI> Fail")));
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyFoundAllyDeadbody Cast<ACharacter_Base> Fail")));
		}
		
	}
}

void AAI_Squad::NotifyLostEnemy(ACharacter_AI* Pawn, float Radius, FVector Center) {
	TArray<AActor*> Allies = SearchNearbyAllies(Pawn->GetActorLocation(), Radius, Pawn);

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr) {
		return;
	}
	
	FNavLocation RandomLocation;
	NavSystem->GetRandomReachablePointInRadius(Center, 250.0f, RandomLocation);
	
	for (INT32 i = 0; i < Allies.Num(); i += 1)	{
		ACharacter_Base* tCharacter = Cast<ACharacter_Base>(Allies[i]);
		if (tCharacter != nullptr) {
			if (tCharacter->IsDead() == false) {
				ACharacter_AIController* AICtrl =  Cast<ACharacter_AIController>(tCharacter->GetController<AAIController>());
				if (AICtrl != nullptr) {

				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyLostEnemy Cast<ACharacter_AI> Fail")));
				}
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::NotifyLostEnemy Cast<ACharacter_Base> Fail")));
		}
		
	}
}

TArray<AActor*> AAI_Squad::SearchNearbyAllies(FVector Center, float Radius, ACharacter_AI* InstigatorActor) {
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(InstigatorActor);
	
	TArray<AActor*> OutActors;
	
	UClass* FilterClass = ACharacter_AI::StaticClass();

	// TArray<FOverlapResult> OverlapResults;
	// FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	//
	// GetWorld()->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity,
	// 	ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(Radius), CollisionQueryParam);

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Center, Radius,
		TraceObjectTypes, FilterClass, IgnoreActors, OutActors);
	
	TArray<AActor*> ReturnActors;
	
	for (INT32 i = 0; i < OutActors.Num(); i += 1) {
		ACharacter_AI* tempCharacter = Cast<ACharacter_AI>(OutActors[i]);
		if (tempCharacter != nullptr) {
			ReturnActors.Add(tempCharacter);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASquadAI::SearchNearbyAllies Cast<ACharacter_AI> Fail")));
		}
	}

	return ReturnActors;
}
