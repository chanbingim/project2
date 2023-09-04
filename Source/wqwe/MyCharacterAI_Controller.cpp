// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacterAI_Controller.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Misc/OutputDeviceNull.h"
#include "Components/CapsuleComponent.h"

#include "AICoverPoint.h"
#include "AI_Search_Point.h"
#include "AI_Search_Point_Manager.h"
#include "CoverSystem.h"
#include "MyCharacterBase.h"
#include "MyCharacterAI.h"
#include "MyCharacter.h"
#include "MyCharacterAI_Squad.h"
#include "WeaponComponent.h"
#include "ARBase.h"
#include "Misc/OutputDeviceNull.h"

const FName AMyCharacterAI_Controller::BB_SelfActor(TEXT("SelfActor"));
const FName AMyCharacterAI_Controller::BB_EnemyActor(TEXT("EnemyActor"));
const FName AMyCharacterAI_Controller::BB_CurrentState(TEXT("CurrentState"));
const FName AMyCharacterAI_Controller::BB_CurrentCombat(TEXT("CurrentCombat"));
const FName AMyCharacterAI_Controller::BB_CurrentCover(TEXT("CurrentCover"));
const FName AMyCharacterAI_Controller::BB_NextVector(TEXT("NextVector"));
const FName AMyCharacterAI_Controller::BB_NextRotator(TEXT("NextRotator"));
const FName AMyCharacterAI_Controller::BB_StartVector(TEXT("StartVector"));
const FName AMyCharacterAI_Controller::BB_StartRotator(TEXT("StartRotator"));

AMyCharacterAI_Controller::AMyCharacterAI_Controller() {
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	AISense_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISense_Sight"));
	AISense_Sight->SightRadius = 1000.0f;
	AISense_Sight->LoseSightRadius = 1000.0f;
	AISense_Sight->PeripheralVisionAngleDegrees = 90.0f;
	AISense_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISense_Sight->DetectionByAffiliation.bDetectFriendlies = true;
	AISense_Sight->DetectionByAffiliation.bDetectNeutrals = true;
	AISense_Sight->SetMaxAge(0.0f);
	
	AISense_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AISense_Hearing"));
	AISense_Hearing->HearingRange = 1000.0f;
	AISense_Hearing->DetectionByAffiliation.bDetectEnemies = true;
	AISense_Hearing->DetectionByAffiliation.bDetectFriendlies = false;
	AISense_Hearing->DetectionByAffiliation.bDetectNeutrals = false;
	AISense_Hearing->SetMaxAge(0.0f);

	AISense_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("AISense_Damage"));
	AISense_Damage->SetMaxAge(0.0f);

	AIPerception->ConfigureSense(*AISense_Hearing);
	AIPerception->ConfigureSense(*AISense_Sight);
	AIPerception->ConfigureSense(*AISense_Damage);
	AIPerception->SetDominantSense(AISense_Sight->GetSenseImplementation());
	
	AAIController::SetGenericTeamId(FGenericTeamId(5));	// Assign to Team 1
}

void AMyCharacterAI_Controller::BeginPlay() {
	Super::BeginPlay();
	
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyCharacterAI_Controller::TargetPerceptionUpdated);
}

void AMyCharacterAI_Controller::OnPossess(APawn* InPawn) {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%s PossessS -> %s"), *this->GetName(), *GetPawn()->GetName()));
	Super::OnPossess(InPawn);

	OwnerPawn = Cast<AMyCharacterAI>(InPawn);

	// 폰의 정보 가져오기
	Active_Range = OwnerPawn->Active_Range;
	
	Sense_Alert_Max = OwnerPawn->Sense_Alert_Max;
	Sense_Search_Increase = OwnerPawn->Sense_Search_Increase;
	Sense_Search_Max = OwnerPawn->Sense_Search_Max;
	Sense_Attack_Max = OwnerPawn->Sense_Attack_Max;

	Distance_Short = OwnerPawn->Distance_Short;
	Distance_Middle = OwnerPawn->Distance_Middle;
	Distance_Long = OwnerPawn->Distance_Long;

	Walk_Speed = OwnerPawn->GetCharacterMovement()->MaxWalkSpeed;
	
	FAISenseID AISense;
	// 시각 정보 갱신
	AISense = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	if (AISense.IsValid()) {
		auto Sense_Sight = GetAIPerception()->GetSenseConfig(AISense);
		if (Sense_Sight != nullptr) {
			auto Sight = Cast<UAISenseConfig_Sight>(Sense_Sight);
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("Update Sight %f, %f - > %f, %f"), Sight->SightRadius, Sight->PeripheralVisionAngleDegrees, OwnerPawn->Sense_Sight_Range, OwnerPawn->Sense_Sight_Degrees));
			
			Sight->SightRadius = FMath::Max(OwnerPawn->Sense_Sight_Range, 0.0f);
			Sight->LoseSightRadius = FMath::Max(OwnerPawn->Sense_Sight_Range, 0.0f);
			Sight->PeripheralVisionAngleDegrees = FMath::Clamp(OwnerPawn->Sense_Sight_Degrees, 0.0f, 180.0f);
			Sight->SetMaxAge(FMath::Max(OwnerPawn->Sense_Sight_Age, 0.0f));
			GetAIPerception()->RequestStimuliListenerUpdate();
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Wrong Sight Config")));
		}
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Wrong Sense ID")));
	}
	
	// 청각 정보 갱신
	AISense = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
	if (AISense.IsValid()) {
		auto Sense_Hearing = GetAIPerception()->GetSenseConfig(AISense);
		if (Sense_Hearing != nullptr) {
			auto Hearing = Cast<UAISenseConfig_Hearing>(Sense_Hearing);
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("Update Hearing %f - > %f"), Hearing->HearingRange, OwnerPawn->Sense_Hearing_Range));

			Hearing->HearingRange = FMath::Max(OwnerPawn->Sense_Hearing_Range, 0.0f);
			Hearing->SetMaxAge(FMath::Max(OwnerPawn->Sense_Hearing_Age, 0.0f));
			GetAIPerception()->RequestStimuliListenerUpdate();
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Wrong Hearing Config")));
		}
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Wrong Sense ID")));
	}

	
	if (GetPawn()) {
		this->AttachToPawn(GetPawn());
	}

	UBlackboardComponent* BlackboardComp = Blackboard;
	if (UseBlackboard(BBAsset, BlackboardComp)) {
		if (RunBehaviorTree(BTAsset)) {
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Character_AIController run behavior tree")));
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%s::%s Run BT Fail"), *GetPawn()->GetName(), *this->GetName()));
		}
	}
	this->Blackboard = BlackboardComp;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAICoverPoint::StaticClass(), FoundActors);
	for (int32 i = 0; i < FoundActors.Num(); i += 1) {
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ASPM %s"), *FoundActors[i]->GetName()));
		CoverManager = Cast<AAICoverPoint>(FoundActors[i]);
	}
}

void AMyCharacterAI_Controller::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("%s Current_State : %s"), *this->GetName(), *UEnum::GetValueAsString(Current_State)));

	// if (CoverPoint.IsValid() == true) {
	// 	DrawDebugSphere(GetWorld(), CoverPoint.Data.Location, 25, 12, FColor::Red, false, DeltaSeconds * 2, 0, 3);
	// }
	
	if (Current_State == E_EnemyState::E_StateDead) {
		return;
	}
	
	if (IsValid(OwnerPawn) == true) {
		if (OwnerPawn->GetDead() == true) {
			StateChange(E_EnemyState::E_StateDead);
			return;
		}
	}
	
	if (IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn()) == true) {
		if (IsValid(Squad) == true) {
			if (Squad->Active == true) {
				if ((Current_State == E_EnemyState::E_StateOff) || (Current_State == E_EnemyState::E_StateComeback)) {
					StartPatrol();
				}
			}
			else {
				if ((Current_State != E_EnemyState::E_StateOff) && (Current_State != E_EnemyState::E_StateComeback)) {
					OwnerPawn->GetCharacterMovement()->MaxWalkSpeed = Walk_Speed / 3;
					StateChange(E_EnemyState::E_StateComeback);
				}
				return;
			}
		}
		else if (IsValid(OwnerPawn) == true) {
			float Range = (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn()->GetActorLocation() - this->GetPawn()->GetActorLocation()).Size();
			//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("State_Current : %s"), *UEnum::GetValueAsString(CurrentState)));
			if (Range > Active_Range) {
				if ((Current_State != E_EnemyState::E_StateOff) && (Current_State != E_EnemyState::E_StateComeback)) {
					StateChange(E_EnemyState::E_StateComeback);
				}
				return;
			}
			else {
				if ((Current_State == E_EnemyState::E_StateOff) || (Current_State == E_EnemyState::E_StateComeback)) {
					StartPatrol();
				}
			}
		}
	}
	else {
		return;
	}

	if (Current_State == E_EnemyState::E_StateOff) {
		return;
	}
	else if (Current_State == E_EnemyState::E_StatePatrol) {
		if (Perception_Sight == true) {
			
		}
		else {
			
		}
	}
	else if (Current_State == E_EnemyState::E_StateAlert) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("Alert %f/%f"), Sense_Current, Sense_Alert_Max));
		
		if (Perception_Sight == true) {
			StartSearch();
		}
		else {
			Sense_Current += -DeltaSeconds;
			if (Sense_Current <= 0) {
				StartPatrol();
			}
			if (IsValid(Search_Point_Manager) == true) {
				if (Search_Point_Manager->PointsLeft() == false) {
					StartPatrol();
				}
			}
		}
	}
	else if (Current_State == E_EnemyState::E_StateSearch) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("Search %f/%f"), Sense_Current, Sense_Search_Max));

		if (Perception_Sight == true) {
			if (IsValid(EnemyActor) == true) {
				float between = (EnemyActor->GetActorLocation() - OwnerPawn->GetActorLocation()).Size();
				// 공식
				Sense_Current += (between < Distance_Short) ?  Sense_Search_Increase * 4 : (between < Distance_Middle) ? Sense_Search_Increase * 2 : Sense_Search_Increase;
				Sense_Current = FMath::Clamp(Sense_Current, 0, Sense_Search_Max);
			
				if (Sense_Current >= Sense_Search_Max) {
					StartAttack();
					if (IsValid(Squad) == true) {
						Squad->RecvFindEnemy(this, EnemyActor);
					}
				}
			}
		}
		else {
			Sense_Current += -Sense_Search_Increase;
			Sense_Alert_Max  = FMath::Clamp(Sense_Current, 0, Sense_Alert_Max);
			if (Sense_Current <= 0) {
				StartAlert(Current_Stimulus.StimulusLocation);
			}
		}
	}
	else if (Current_State == E_EnemyState::E_StateAttack) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("Attack %f/%f"), Sense_Current, Sense_Attack_Max));

		if (Perception_Sight == true) {
			Sense_Current = Sense_Attack_Max;
			Sense_Current = FMath::Clamp(Sense_Current, 0, Sense_Attack_Max);
		}
		else {
			Sense_Current += -DeltaSeconds;
			Sense_Current = FMath::Clamp(Sense_Current, 0, Sense_Attack_Max);
			if (IsValid(Squad) == true) {
				if ((Sense_Current <= 0) && (Squad->SendIsAnyoneAttack() == false)) {
					Squad->RecvLostEnemy();
					if (IsValid(EnemyActor) == true) {
						RunSearchPointManager(EnemyActor->GetActorLocation());
						StartAlert(EnemyActor->GetActorLocation());
					}
				}
			}
			else {
				if (Sense_Current <= 0) {
					RunSearchPointManager(EnemyActor->GetActorLocation());
					StartAlert(EnemyActor->GetActorLocation());
				}
			}
		}

		if (IsValid(EnemyActor) == true) {
			// FVector vec = EnemyActor->GetActorLocation() - OwnerPawn->GetActorLocation();
			// float xDis = FVector(vec.X, vec.Y, 0).Size();
			// float yDis = EnemyActor->GetActorLocation().Z - OwnerPawn->GetActorLocation().Z;
			// float Angle = FMath::RadiansToDegrees(atan(yDis / xDis));
			//
			// OwnerPawn->GetAimCamera()->SetRelativeRotation(FRotator(Angle, 0, 0));
			// OwnerPawn->GunFire();
			// OwnerPawn->LookAtPoint(EnemyActor->GetActorLocation(), 1.0f);
		}
	}
	else if (Current_State == E_EnemyState::E_StateComeback) {
		if (Perception_Sight == true) {
			
		}
		else {
			
		}
	}

	return;
}

void AMyCharacterAI_Controller::Destroyed() {
	Super::Destroyed();

	if (IsValid(Squad) == true) {
		if (Squad->GetSquadArray().Num() <= 1) {
			if (IsValid(Search_Point_Manager) == true) {
				Search_Point_Manager->Destroy();
			}
		}
	}
	else {
		if (IsValid(Search_Point_Manager) == true) {
			Search_Point_Manager->Destroy();
		}
	}
}

ETeamAttitude::Type AMyCharacterAI_Controller::GetTeamAttitudeTowards(const AActor& Other) const {
	//return Super::GetTeamAttitudeTowards(Other);
	
	//Other Actor가 Pawn이면
	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {
		//Other의 TeamAgent가 있으면
		if (const IGenericTeamAgentInterface* TeamAgent	= Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))	{
			FGenericTeamId const OtherTeamID = TeamAgent->GetGenericTeamId();
			//TeamAgent가 기본값이면
			if (OtherTeamID == FGenericTeamId::NoTeam) {
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, FString::Printf(TEXT("No Team")));
				return ETeamAttitude::Neutral;
			}
			else {
				ETeamAttitude::Type const Rtn = Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
				//GEngine->AddOnScreenDebugMessage(-1, 10, Rtn == ETeamAttitude::Hostile ? FColor::Red : FColor::Yellow, FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(Rtn)));
				return Rtn;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, FString::Printf(TEXT("GetTeamAttitudeTowards::No TeamAgentInterface")));
		return ETeamAttitude::Neutral;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, FString::Printf(TEXT("GetTeamAttitudeTowards::Not Pawn")));
	return ETeamAttitude::Neutral;
}

void AMyCharacterAI_Controller::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) {
	Super::ActorsPerceptionUpdated(UpdatedActors);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ActorsPerceptionUpdated")));

	for (auto Actor : UpdatedActors) {
		AAI_Search_Point* Point = Cast<AAI_Search_Point>(Actor);
		if (Point) {
			Point->ReleasePool();
		}
	}
}

void AMyCharacterAI_Controller::TargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus) {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("TargetPerceptionUpdated")));
	
	if ((Current_State == E_EnemyState::E_StateOff) || (Current_State == E_EnemyState::E_StateDead)) {
		return;
	}

	AMyCharacterBase* CurrentStimulusCharacter = Cast<AMyCharacterBase>(Actor);
	if (CurrentStimulusCharacter == nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("AMyCharacterAI_Controller::TargetPerceptionUpdated Cast<ACharacter_Base> Fail")));
		return;
	}
	
	ETeamAttitude::Type const type = GetTeamAttitudeTowards(*Actor);
	//시야
	auto const SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	
	if (SensedClass == UAISense_Sight::StaticClass()) {
		//시야 들어옴
		if (Stimulus.WasSuccessfullySensed() == true) {
			if (Cast<AMyCharacter>(Actor) != nullptr) {
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("In type %s"), *UEnum::GetValueAsString(type)));
			}
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("View In")));			
			//OwnerPawn->LookAtPoint(Stimulus.StimulusLocation);

			//시야에 들어온게 적이면
			if (type == ETeamAttitude::Hostile)	{
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("View Hostile")));
				Perception_Sight = true;

				GetBlackboardComponent()->SetValueAsVector(AMyCharacterAI_Controller::BB_NextVector, Stimulus.StimulusLocation);
				Current_Stimulus = Stimulus;
				
				GetBlackboardComponent()->SetValueAsObject(AMyCharacterAI_Controller::BB_EnemyActor, CurrentStimulusCharacter);
				EnemyActor = Cast<AMyCharacter>(GetBlackboardComponent()->GetValueAsObject(AMyCharacterAI_Controller::BB_EnemyActor));
				
				if (Current_State != E_EnemyState::E_StateAttack) {
					StartSearch();
				}
			}
			//시야에 들어온게 적이 아니면
			else {
				if (Current_State != E_EnemyState::E_StateAttack) {
					//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::Printf(TEXT("View Neut")));
					if (CurrentStimulusCharacter->GetDead() == true) {
						if (CurrentStimulusCharacter->bDeathFound == false) {
							CurrentStimulusCharacter->bDeathFound = true;
							RunSearchPointManager(Stimulus.StimulusLocation);
							StartAlert(Stimulus.StimulusLocation);
						}
					}
				}
			}
		}
		//시야 벗어남
		else {
			if (Cast<AMyCharacter>(Actor) != nullptr) {
				//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Out type %s"), *UEnum::GetValueAsString(type)));
			}
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("View Out")));
			ReMoveWidget();
			
			if (type == ETeamAttitude::Hostile)	{
				Perception_Sight = false;
				if (Current_State != E_EnemyState::E_StateAttack) {
					Current_Stimulus = Stimulus;
					RunSearchPointManager(Stimulus.StimulusLocation);
					StartAlert(Stimulus.StimulusLocation);
				}
			}
		}
	}
	//소리
	if (SensedClass == UAISense_Hearing::StaticClass()) {
		if (Stimulus.WasSuccessfullySensed() == true) {
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, FString::Printf(TEXT("Sound")));
			CurrentStimulusCharacter = nullptr;
			
			//적대적 존재의 소리면
			if (type == ETeamAttitude::Hostile) {
				if (Current_State != E_EnemyState::E_StateAttack) {
					Current_Stimulus = Stimulus;
					if (IsValid(Search_Point_Manager) == true) {
						FVector ForwardVector = GetPawn()->GetActorForwardVector().GetSafeNormal();
						FVector BetweenVector = (GetPawn()->GetActorLocation() - Stimulus.StimulusLocation).GetSafeNormal();
						float Dot = FVector::DotProduct(ForwardVector, BetweenVector);
						float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
						
						if (abs(Angle - 180) > OwnerPawn->Sense_Sight_Degrees / 2) {
							RunSearchPointManager(Stimulus.StimulusLocation);
							StartAlert(Stimulus.StimulusLocation);
						}
					}
					else {
						RunSearchPointManager(Stimulus.StimulusLocation);
						StartAlert(Stimulus.StimulusLocation);
					}
				}
			}
		}
	}
	if (SensedClass == UAISense_Damage::StaticClass()) {
		if (Stimulus.WasSuccessfullySensed() == true) {
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Damaged")));
			if (type == ETeamAttitude::Hostile) {
				if (Current_State != E_EnemyState::E_StateAttack) {
					Current_Stimulus = Stimulus;
					if (IsValid(Search_Point_Manager) == true) {
						FVector ForwardVector = GetPawn()->GetActorForwardVector().GetSafeNormal();
						FVector BetweenVector = (GetPawn()->GetActorLocation() - Stimulus.StimulusLocation).GetSafeNormal();
						float Dot = FVector::DotProduct(ForwardVector, BetweenVector);
						float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
						
						if (abs(Angle - 180) > OwnerPawn->Sense_Sight_Degrees / 2) {
							RunSearchPointManager(Stimulus.StimulusLocation);
							StartAlert(Stimulus.StimulusLocation);
						}
					}
					else {
						RunSearchPointManager(Stimulus.StimulusLocation);
						StartAlert(Stimulus.StimulusLocation);
					}
				}
			}
		}
	}
}

void AMyCharacterAI_Controller::StateChange(E_EnemyState NewState) {
	Current_State = NewState;
	Blackboard->SetValueAsEnum(BB_CurrentState, (uint8)Current_State);
	OwnerPawn->ShowIcon(NewState);

	if (Timer_GunFire.IsValid() == true) {
		GetWorldTimerManager().ClearTimer(Timer_GunFire);
	}
	
	if ((NewState == E_EnemyState::E_StatePatrol) || (NewState == E_EnemyState::E_StateAttack)) {
		ClearFocus(EAIFocusPriority::Gameplay);
		if (IsValid(Search_Point_Manager) == true) {
			Search_Point_Manager->Destroy();
		}
	}

	if (NewState == E_EnemyState::E_StateSearch) {
		Widget_Call();
	}

	if (NewState == E_EnemyState::E_StateAttack) {
		SetCurrentCover(E_EnemyCombatCoverType::Cover_None);
	}

	if (NewState == E_EnemyState::E_StateDead) {
		if (IsValid(OwnerPawn) == true) {
			for (auto Gun : OwnerPawn->GetWeaponComponent()->GunSlot) {
				//Gun->DropGun();
				if (IsValid(Gun) == false) {
					continue;
				}
				Gun->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Gun->DropGun();
			}
		}
		
	}
}

void AMyCharacterAI_Controller::StartPatrol() {
	StateChange(E_EnemyState::E_StatePatrol);

	GetBlackboardComponent()->SetValueAsObject(BB_EnemyActor, nullptr);
	EnemyActor = nullptr;
	
	OwnerPawn->GetCharacterMovement()->MaxWalkSpeed = Walk_Speed / 3;
}

void AMyCharacterAI_Controller::StartAlert(FVector Point) {
	StateChange(E_EnemyState::E_StateAlert);
	
	OwnerPawn->LookAtPoint(Point, 1.0f);
	OwnerPawn->GetCharacterMovement()->MaxWalkSpeed = Walk_Speed;
	Sense_Current = Sense_Alert_Max;

	GetBlackboardComponent()->SetValueAsVector(BB_NextVector, Point);
}

void AMyCharacterAI_Controller::StartSearch() {
	StateChange(E_EnemyState::E_StateSearch);
	OwnerPawn->GetCharacterMovement()->MaxWalkSpeed = Walk_Speed;
	Sense_Current = 0.0f;
}

void AMyCharacterAI_Controller::StartAttack() {
	StateChange(E_EnemyState::E_StateAttack);
	OwnerPawn->GetCharacterMovement()->MaxWalkSpeed = Walk_Speed;
	Sense_Current = Sense_Attack_Max;
}

void AMyCharacterAI_Controller::SetSearchPointManager(AAI_Search_Point_Manager* Manager) {
	Search_Point_Manager = Manager;
}

void AMyCharacterAI_Controller::RunSearchPointManager(FVector Point) {
	if (IsValid(Search_Point_Manager) == true) {
		BrainComponent->RestartLogic();

		Search_Point_Manager->SetActorLocation(Point);
		Search_Point_Manager->ClearSearchPoint();
		Search_Point_Manager->GenerateSearchPoint(OwnerPawn->Alert_Size, OwnerPawn->Alert_Between);
	}
	else {
		FVector vec = Point;
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = GetInstigator();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		Search_Point_Manager = GetWorld()->SpawnActor<AAI_Search_Point_Manager>(Search_Point_Manager_Class, vec, FRotator(0, 0, 0), Params);
		Search_Point_Manager->GenerateSearchPoint(OwnerPawn->Alert_Size, OwnerPawn->Alert_Between);

		if (IsValid(Squad) == true) {
			Squad->RecvPointManager(this, Search_Point_Manager);
		}
		else {
			Search_Point_Manager->AI_Array.Add(this);
		}
	}
}

E_EnemyState AMyCharacterAI_Controller::GetCurrentState() const {
	return Current_State;
}

void AMyCharacterAI_Controller::SetCurrentState(E_EnemyState NewState) {
	Current_State = NewState;
	Blackboard->SetValueAsEnum(BB_CurrentState, (uint8)Current_State);
	OwnerPawn->ShowIcon(NewState);
	
	if ((NewState == E_EnemyState::E_StatePatrol) || (NewState == E_EnemyState::E_StateAttack)) {
		ClearFocus(EAIFocusPriority::Gameplay);
		if (IsValid(Search_Point_Manager) == true) {
			Search_Point_Manager->Destroy();
		}
	}

	return;
}


E_EnemyCombatCoverType AMyCharacterAI_Controller::GetCurrentCover() const {
	return Current_Cover;
}

void AMyCharacterAI_Controller::SetCurrentCover(E_EnemyCombatCoverType NewCover) {
	Current_Cover = NewCover;
	GetBlackboardComponent()->SetValueAsEnum(AMyCharacterAI_Controller::BB_CurrentCover, (uint8)NewCover);
	return;
}

float AMyCharacterAI_Controller::GetSenseCurrent() const {
	return Sense_Current;
}

void AMyCharacterAI_Controller::SetSenseCurrent(float Input) {
	Sense_Current = Input;
}

TTuple<float, float> AMyCharacterAI_Controller::GetSightConfig() {
	return MakeTuple(OwnerPawn->Sense_Sight_Range, OwnerPawn->Sense_Sight_Degrees);
}

bool AMyCharacterAI_Controller::GetTarget() {
	if (EnemyActor != nullptr) {
		return true;
	}
	return false;
}

void AMyCharacterAI_Controller::GetAmsal(APawn* InPawn, FVector AILoc) {
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Do Amsal")));
	StateChange(E_EnemyState::E_StateDead);
	BrainComponent->StopLogic("Dead");
	GetWorldTimerManager().SetTimer(Timer_Amsal, this, &AMyCharacterAI_Controller::FinishAmsal, 6.5f, false);

	OwnerPawn->GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	OwnerPawn->SetActorLocation(AILoc);
	OwnerPawn->SetActorRotation(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorRotation());
	OwnerPawn->PlayAmsalMontage();
	OwnerPawn->GetMesh()->SetCollisionProfileName("NoCollision");
}

void AMyCharacterAI_Controller::FinishAmsal() {
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Amsal")));
	GetWorldTimerManager().ClearTimer(Timer_Amsal);

	OwnerPawn->GetDamage(OwnerPawn->HP_Max);
	OwnerPawn->GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
}

void AMyCharacterAI_Controller::LookPlayer() {
	if (IsValid(EnemyActor) == true) {
		OwnerPawn->LookAtPoint(EnemyActor->GetActorLocation(), 3.0f);
	}
}

void AMyCharacterAI_Controller::StartWeaponFire() {
	if (IsValid(OwnerPawn) == true) {
		CoverManager->PickCover(CoverPoint, this, 5000.0f);
		if (IsValid(OwnerPawn->GetWeaponComponent()->CurrentEquipWeapon) == true) {
			if (OwnerPawn->GetWeaponComponent()->CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle) {
				AARBase* Rifle = Cast<AARBase>(OwnerPawn->GetWeaponComponent()->CurrentEquipWeapon);
				if (Rifle != nullptr) {
					Rifle->Selector = ESelectorType::Burst;
				}
			}
		}

		OwnerPawn->GetWeaponComponent()->GunFire();
	}
	
	if (Timer_GunFire.IsValid() == false) {
		GetWorldTimerManager().SetTimer(Timer_GunFire, this, &AMyCharacterAI_Controller::StartWeaponFire, 0.5f, true, 0.f);
		GetWorldTimerManager().SetTimer(Timer_PlayerLook, this, &AMyCharacterAI_Controller::LookPlayer, 0.1f, true, 0.f);
	}
}

void AMyCharacterAI_Controller::StopWeaponFire() {
	GetWorldTimerManager().ClearTimer(Timer_GunFire);
	GetWorldTimerManager().ClearTimer(Timer_PlayerLook);
}

//void AMyCharacterAI_Controller::GetAmsal(APawn* InPawn)
//{
//	AMyCharacterAI* OwnerPawn2 = Cast<AMyCharacterAI>(InPawn);
//
//	OwnerPawn2->PlayAmsalMontage();
//}
