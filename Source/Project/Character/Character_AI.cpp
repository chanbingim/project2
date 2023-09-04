// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_AI.h"

#include "Character_AIController.h"
#include "AI/AI_Squad.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "AI/AI_Patrol_Path.h"
#include "NavigationSystem.h"
#include "AI/Widget_AI_Reaction.h"
#include "Components/BillboardComponent.h"
#include "Components/TimeLineComponent.h"
#include "Components/WidgetComponent.h"

ACharacter_AI::ACharacter_AI() {
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	WidgetIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetIcon"));
	WidgetIcon->SetupAttachment(RootComponent);
	WidgetIcon->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	
	BillboardIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardIcon"));
	BillboardIcon->SetupAttachment(RootComponent);
	BillboardIcon->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));

	AIControllerClass = ACharacter_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACharacter_AI::BeginPlay() {
	Super::BeginPlay();

	Patrol_Start_Location = this->GetActorLocation();
	Patrol_Start_Rotator = this->GetActorRotation();
	Patrol_Spline_Index = Patrol_Spline_Init;
	
	//AI 캐릭터의 컨트롤러 가져오기
	ACharacter_AIController* AICtrl = Cast<ACharacter_AIController>(GetController());
	if (AICtrl != nullptr) {
		if (Squad != nullptr) {
			AICtrl->Squad = this->Squad;
			Squad->Squad_Array.Add(AICtrl);

			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("%s"), *Squad->GetName()));
		}

		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("%s Controller : %s"), *this->GetName(), *GetController()->GetName()));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("ACharacter_AI::BeginPlay Cast<ACharacter_AIController> Fail")));
	}
	
	if (Curve) {
		FOnTimelineFloat TimelineProgress;

		TimelineProgress.BindUFunction(this, FName("TimelineProcess"));
		CurveTimeline.AddInterpFloat(Curve, TimelineProgress);
		CurveTimeline.SetLooping(false);
	}
}

void ACharacter_AI::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	CurveTimeline.TickTimeline(DeltaTime);

	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), Weapon_Range, 20, FColor(0,0,0), false, DeltaTime * 2, 0, 1);
}

FTransform ACharacter_AI::GetNextPatrolPosition() {
	FVector vec = FVector(0, 0, 0);
	FRotator rot = FRotator(0, 0, 0);

	
	switch(Patrol_Type) {
	case E_EnemyPatrolType::Patrol_Spline: {
			TTuple<FVector, int32, bool> tpl = Patrol_Spline->GetNextPosition(Patrol_Spline_Index, Patrol_Forward);
			vec = tpl.Get<0>();
			Patrol_Spline_Index = tpl.Get<1>();
			Patrol_Forward = tpl.Get<2>();
			float zrot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), vec).Yaw;
			rot = FRotator(0, zrot, 0);
		}
		break;
	case E_EnemyPatrolType::Patrol_Random: {
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this->GetWorld());
			FNavLocation NextPatrol;
			
			//없으면 종료
			if (NavSystem == nullptr) {
				break;
			}
			
			if (NavSystem->GetRandomPointInNavigableRadius(Patrol_Start_Location, Patrol_Random_Range, NextPatrol)) {
				vec = NextPatrol;
			}
			
			float zrot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), vec).Yaw;
			rot = FRotator(0, zrot, 0);
		}
		break;
	case E_EnemyPatrolType::Patrol_Stay:
		vec = Patrol_Start_Location;
		rot = Patrol_Start_Rotator;
		break;
	default: break;
	}

	return FTransform(rot, vec, FVector(1.0f, 1.0f, 1.0f));
}

void ACharacter_AI::LookAtPoint(FVector Point) {
	Rotator_Start = this->GetActorRotation();
	Rotator_End = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Point);
	
	CurveTimeline.PlayFromStart();
}

void ACharacter_AI::TimelineProcess() {
	float yaw = UKismetMathLibrary::REase(Rotator_Start, Rotator_End, CurveTimeline.GetPlaybackPosition(), true, EEasingFunc::CircularOut).Yaw;
	
	SetActorRotation(FRotator(0, yaw, 0), ETeleportType::None);
}

void ACharacter_AI::ShowIcon(E_EnemyState State) {
	UWidget_AI_Reaction* tWidget = Cast<UWidget_AI_Reaction>(WidgetIcon->GetUserWidgetObject());
	tWidget->IconPlay(State);
}

