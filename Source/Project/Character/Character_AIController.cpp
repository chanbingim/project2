// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_AIController.h"

#include "AI_Search_Point_Manager.h"
#include "Character_Base.h"
#include "Character_Ai.h"
#include "Character_Player.h"
#include "AI_Squad.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

const FName ACharacter_AIController::BB_EnemyActor(TEXT("EnemyActor"));
const FName ACharacter_AIController::BB_MysteryActor(TEXT("MysteryActor"));
const FName ACharacter_AIController::BB_CurrentState(TEXT("CurrentState"));
const FName ACharacter_AIController::BB_NextVector(TEXT("NextVector"));
const FName ACharacter_AIController::BB_NextRotator(TEXT("NextRotator"));

ACharacter_AIController::ACharacter_AIController() {
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	//AIPerception->SetDominantSense(UAISense_Sight::StaticClass());
	
	AAIController::SetGenericTeamId(FGenericTeamId(5));	// Assign to Team 1
	
	// static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	// if (BBObject.Succeeded()) {
	// 	BBAsset = BBObject.Object;
	// }
	// static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	// if (BTObject.Succeeded()) {
	// 	BTAsset = BTObject.Object;
	// }
}

void ACharacter_AIController::BeginPlay() {
	Super::BeginPlay();
	
	OwnerPawn = Cast<ACharacter_AI>(GetPawn());
	
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACharacter_AIController::TargetPerceptionUpdated);
}

void ACharacter_AIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	
	if (GetPawn()) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("%s Owner : %s"), *this->GetName(), *GetPawn()->GetName()));
	}

	UBlackboardComponent* BlackboardComp = Blackboard;
	if (UseBlackboard(BBAsset, BlackboardComp)) {
		if (!RunBehaviorTree(BTAsset)) {
			UE_LOG(LogTemp, Warning, TEXT("Character_AIController couldn't run behavior tree!"));
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("Character_AIController couldn't run behavior tree!")));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("Character_AIController run behavior tree")));
		}
	}
	this->Blackboard = BlackboardComp;
}

void ACharacter_AIController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red,
		FString::Printf(TEXT("State : %s"), *UEnum::GetValueAsString(CurrentState)));

	if (bPerception == true) {
		switch (CurrentState) {  
		case E_EnemyState::E_StateSearch: {
				ACharacter_Player* Other = Cast<ACharacter_Player>(GetBlackboardComponent()->GetValueAsObject(ACharacter_AIController::BB_MysteryActor));
				if (Other) {
					OwnerPawn->LookAtPoint(Other->GetActorLocation());
				}
				SearchTimer += 2;
				SearchTimer = FMath::Clamp(SearchTimer, 0, SearchTimer_Max);
				if (SearchTimer == SearchTimer_Max) {
					//GetBlackboardComponent()->SetValueAsObject(ACharacter_AIController::BB_NextVector, Actor);
					//StateChange(E_EnemyState::E_StateAttack);
				}
			}
			break;
		case E_EnemyState::E_StateAttack:
			if (Player)	{
				GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Player->GetActorLocation());
			}
			break;
		default:
			break;
		}
	}
	
	
	if (CurrentState == E_EnemyState::E_StateAlert) {
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red,
		FString::Printf(TEXT("Alert %d/%d"), AlertTimer, AlertTimer_Max));
		
		if (AlertTimer > 0) {
			AlertTimer += -1;
		}
		else {
			AlertTimer = AlertTimer_Max;
			StateChange(E_EnemyState::E_StatePatrol);
		}

		// ACharacter_Player* Player = Cast<ACharacter_Player>(GetBlackboardComponent()->GetValueAsObject(ACharacter_AIController::BB_NextVector));
		// ACharacter_AI* OwnerPawn = Cast<ACharacter_AI>(GetPawn());
		// if (Player) {
		// 	OwnerPawn->LookAtPoint(Player->GetActorLocation());
		// 	GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Player->GetActorLocation());
		// }
	}
	else if (CurrentState == E_EnemyState::E_StateSearch) {
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red,
		FString::Printf(TEXT("Search %d/%d"), SearchTimer, SearchTimer_Max));
		SearchTimer += -1;
		SearchTimer = FMath::Clamp(SearchTimer, 0, SearchTimer_Max);
		if (SearchTimer == 0) {
			StateChange(E_EnemyState::E_StateAlert);
		}
	}
}

ETeamAttitude::Type ACharacter_AIController::GetTeamAttitudeTowards(const AActor& Other) const {
	//return Super::GetTeamAttitudeTowards(Other);

	//Other Actor가 Pawn이면
	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {

		//Other의 TeamAgent가 있으면
		if (const IGenericTeamAgentInterface* TeamAgent	= Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))	{
			FGenericTeamId const OtherTeamID = TeamAgent->GetGenericTeamId();

			//TeamAgent가 기본값이면
			if (OtherTeamID == FGenericTeamId::NoTeam) {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,
				FString::Printf(TEXT("No Team")));
				return ETeamAttitude::Neutral;
			}
			else {
				ETeamAttitude::Type const Rtn = Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
				GEngine->AddOnScreenDebugMessage(-1, 10, Rtn == ETeamAttitude::Hostile ? FColor::Red : FColor::Yellow,
					FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(Rtn)));
				return Rtn;
			}
		}

		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,
		FString::Printf(TEXT("GetTeamAttitudeTowards::No TeamAgentInterface")));
		return ETeamAttitude::Neutral;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,
		FString::Printf(TEXT("GetTeamAttitudeTowards::Not Pawn")));
	return ETeamAttitude::Neutral;
}

void ACharacter_AIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) {
	Super::ActorsPerceptionUpdated(UpdatedActors);
	/*
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("actors %d"), UpdatedActors.Num()));

	for (int i = 0; i < UpdatedActors.Num(); i += 1) {
		AActor* Actor = UpdatedActors[i];
	
		FActorPerceptionBlueprintInfo PerceptionInfo;
		GetAIPerception()->GetActorsPerception(Actor, PerceptionInfo);
	
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("Perception")));
		
		for (int j = 0; j < PerceptionInfo.LastSensedStimuli.Num(); j += 1) {
			if (j == 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("View")));
			}
			else if (j == 1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("Sound")));
			}
		}
	}
	*/
}

void ACharacter_AIController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus) {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("TargetPerceptionUpdated")));
	
	/*
	FActorPerceptionBlueprintInfo PerceptionInfo;
	GetAIPerception()->GetActorsPerception(Actor, PerceptionInfo);
	
	for (int i = 0; i < PerceptionInfo.LastSensedStimuli.Num(); i += 1) {
	
		auto Info = PerceptionInfo.LastSensedStimuli[i];
		auto SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Info);
	
		if (SensedClass == UAISense_Sight::StaticClass()) {
			if (Info.WasSuccessfullySensed() == true) {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
				 FString::Printf(TEXT("View")));
			}
		}
		else if (SensedClass == UAISense_Hearing::StaticClass()) {
			if (Info.WasSuccessfullySensed() == true) {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
				 FString::Printf(TEXT("Sound")));
			}
		}
	}
	*/

	StimulusCharacter = Cast<ACharacter_Base>(Actor);
	if (IsValid(StimulusCharacter) == false) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("ACharacter_AIController::TargetPerceptionUpdated Cast<ACharacter_Base> Fail")));
		return;
	}
	CurrentStimulus = Stimulus;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("update %s"), *CurrentStimulus.StimulusLocation.ToString()));

	ETeamAttitude::Type const type = GetTeamAttitudeTowards(*Actor);
	//시야
	auto const SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	if (SensedClass == UAISense_Sight::StaticClass()) {
		//시야 들어옴
		if (Stimulus.WasSuccessfullySensed() == true) {
			bPerception = true;
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green,
			FString::Printf(TEXT("View In")));
			
			//OwnerPawn->LookAtPoint(Stimulus.StimulusLocation);

			//시야에 들어온게 적이면
			if (type == ETeamAttitude::Hostile)	{
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red,
				FString::Printf(TEXT("View Hostile")));
				
				switch(CurrentState) {
				case E_EnemyState::E_StatePatrol:
					StateChange(E_EnemyState::E_StateAlert);
					break;
				case E_EnemyState::E_StateAlert:
					StateChange(E_EnemyState::E_StateSearch);
					GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Stimulus.StimulusLocation);
					break;
				case E_EnemyState::E_StateSearch:
					GetBlackboardComponent()->SetValueAsObject(ACharacter_AIController::BB_MysteryActor, Actor);
					GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Stimulus.StimulusLocation);
					//SearchTimer += 2;
					//SearchTimer = FMath::Clamp(SearchTimer, 0, SearchTimer_Max);
					// if (SearchTimer == SearchTimer_Max) {
					// 	Player = Cast<ACharacter_Player>(Actor);
					// 	GetBlackboardComponent()->SetValueAsObject(ACharacter_AIController::BB_NextVector, Actor);
					// 	StateChange(E_EnemyState::E_StateAttack);
					// }
					break;
				case E_EnemyState::E_StateAttack:
					Player = Cast<ACharacter_Player>(Actor);
					GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Stimulus.StimulusLocation);
					GetBlackboardComponent()->SetValueAsObject(ACharacter_AIController::BB_NextVector, Actor);
					break;
				default:
					break;
				}

				if (OwnerPawn->Weapon_Range > (GetPawn()->GetActorLocation() - Actor->GetActorLocation()).Length()) {
					if (CurrentState != E_EnemyState::E_StateAttack) {
						StateChange(E_EnemyState::E_StateAttack);
					}
				}
				
				if (StimulusCharacter->IsDead() == true) {
					//EnemyIsDead();
				}
			}
			//시야에 들어온게 적이 아니면
			else {
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green,
				FString::Printf(TEXT("View Not Hostile")));

				if (StimulusCharacter->IsDead() == true) {
					//FindAllyDeadbody(Stimulus.StimulusLocation);
				}
			}
		}
		//시야 벗어남
		else {
			bPerception = false;
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green,
			FString::Printf(TEXT("View Out")));
			if (type == ETeamAttitude::Hostile)	{
				//EnemyLost(Stimulus.GetAge(), Stimulus.StimulusLocation);
			}
		}
	}
	//소리
	else if (SensedClass == UAISense_Hearing::StaticClass()) {
		if (Stimulus.WasSuccessfullySensed() == true) {
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow,
				FString::Printf(TEXT("Sound")));

			//OwnerPawn->CurveTimeline.PlayFromStart();

			//적대적 존재의 소리면
			if (type == ETeamAttitude::Hostile) {
				StartAlert(Stimulus.StimulusLocation);
			}
			else {
				
			}
		}
	}
}

void ACharacter_AIController::StateChange(E_EnemyState NewState) {
	if ((CurrentState == E_EnemyState::E_StatePatrol || CurrentState == E_EnemyState::E_StateSearch) && NewState == E_EnemyState::E_StateAlert) {
		AlertTimer = AlertTimer_Max;
	}

	if (NewState == E_EnemyState::E_StatePatrol) {
		if (IsValid(Search_Point_Manager) == true) {
			Search_Point_Manager->Destroy();
		}
	}
	SearchTimer = 0;

	CurrentState = NewState;
	OwnerPawn->ShowIcon(NewState);

	//OwnerPawn.ChangeState(NewState);
	Blackboard->SetValueAsEnum(BB_CurrentState, (uint8)CurrentState);
	
	if (NewState == E_EnemyState::E_StatePatrol) {
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void ACharacter_AIController::StartAlert(FVector Point) {

	OwnerPawn->LookAtPoint(Point);
	StateChange(E_EnemyState::E_StateAlert);

	FVector vec = Point;
	FRotator rot = FRotator(0, 0, 0);
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (IsValid(Search_Point_Manager) == true) {
		Search_Point_Manager->Destroy();
	}
	Search_Point_Manager = GetWorld()->SpawnActor<AAI_Search_Point_Manager>(vec, rot, Params);

	GetBlackboardComponent()->SetValueAsVector(ACharacter_AIController::BB_NextVector, Point);
}

void ACharacter_AIController::FinishAlert() {
	StateChange(E_EnemyState::E_StatePatrol);
}


