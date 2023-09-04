// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacterAI.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimeLineComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MyCharacterAI_Controller.h"
#include "MyCharacter.h"
#include "MyCharacterAI_Squad.h"
#include "WeaponComponent.h"
#include "AI/AI_Patrol_Path.h"
#include "AI/Widget_AI_Reaction.h"

AMyCharacterAI::AMyCharacterAI() {
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	WidgetIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetIcon"));
	WidgetIcon->SetupAttachment(RootComponent);
	WidgetIcon->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	// Create a camera boom (pulls in towards the player if there is a collision)
	AimCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
	AimCamera->SetupAttachment(RootComponent);
	AimCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

	CloseBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseBox"));
	CloseBox->SetupAttachment(GetMesh());
	CloseBox->SetRelativeLocation(FVector(0.0f, 0.0f, 88.0f));
	CloseBox->SetBoxExtent(FVector(68.0f, 20.0f, 88.0f));
	
	AIControllerClass = AMyCharacterAI_Controller::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Weapon_Component = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon_Component"));
	GunSlot.SetNum((uint8)EWeaponSlot::End - 1);
}

void AMyCharacterAI::BeginPlay() {
	Super::BeginPlay();

	CloseBox->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacterAI::OnOverlapBegin);

	// 캐릭터 지상에 붙여서 시작하기
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 1000.0f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params)) {
		FVector FloorLocation = HitResult.Location;
		SetActorLocation(FloorLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
		Patrol_Start_Location	= this->GetActorLocation();
	}
	Patrol_Start_Rotator	= this->GetActorRotation();
	Patrol_Spline_Index		= Patrol_Spline_Init;

	
	//AI 캐릭터의 컨트롤러 가져오기
	AI_Controller = Cast<AMyCharacterAI_Controller>(GetController());
	if (AI_Controller != nullptr) {
		if (IsValid(Squad) == true) {
			AI_Controller->Squad = this->Squad;
			Squad->AddSquadArray(AI_Controller);
			Alert_Size = Squad->Search_Size;
			Alert_Between = Squad->Search_Between;
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%s"), *Squad->GetName()));
		}
		
		AI_Controller->GetBlackboardComponent()->SetValueAsObject(AMyCharacterAI_Controller::BB_SelfActor, this);
		AI_Controller->GetBlackboardComponent()->SetValueAsEnum(AMyCharacterAI_Controller::BB_CurrentState, (uint8)E_EnemyState::E_StateOff);
		AI_Controller->GetBlackboardComponent()->SetValueAsEnum(AMyCharacterAI_Controller::BB_CurrentCombat, (uint8)Combat_Type);
		
		AI_Controller->GetBlackboardComponent()->SetValueAsVector(AMyCharacterAI_Controller::BB_StartVector, Patrol_Start_Location);
		AI_Controller->GetBlackboardComponent()->SetValueAsRotator(AMyCharacterAI_Controller::BB_StartRotator, Patrol_Start_Rotator);
		AI_Controller->GetBlackboardComponent()->SetValueAsVector(AMyCharacterAI_Controller::BB_NextVector, Patrol_Start_Location);
		AI_Controller->GetBlackboardComponent()->SetValueAsRotator(AMyCharacterAI_Controller::BB_NextRotator, Patrol_Start_Rotator);
		
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%s Controller : %s"), *this->GetName(), *GetController()->GetName()));
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ACharacter_AI::BeginPlay Cast<ACharacter_AIController> Fail")));
	}
	
	//해당지점으로 회전하는 타임라인 설정
	if (TL_Look_Curve != nullptr) {
		TL_Look_UpdateDelegate.BindUFunction(this, FName("TL_Look_UpdateFunc"));
		TL_Look.AddInterpFloat(TL_Look_Curve, TL_Look_UpdateDelegate);

		TL_Look_FinishDelegate.BindUFunction(this, FName("TL_Look_FinishFunc"));
		TL_Look.SetTimelineFinishedFunc(TL_Look_FinishDelegate);

		float Min = 0, Max = 0;
		TL_Look_Curve->GetTimeRange(Min,Max);
		TL_Look.SetTimelineLength(Max);
		TL_Look.SetLooping(false);
	}

	Weapon_Component->InitalizeWeaponComponent(GetAimCamera(), GunSlot[0], GunSlot[1], GunSlot[2], false);
}

void AMyCharacterAI::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	TL_Look.TickTimeline(DeltaTime);

	if (bDead == true) {
		Death_Time += -DeltaTime;
		if (Death_Time < 0) {
			if (GetController() != nullptr) {
				Destroy();
			}
		}
	}
	
	//DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), Weapon_Range, 20, FColor(0,0,0), false, DeltaTime * 2, 0, 1);
}

void AMyCharacterAI::Destroyed() {
	Super::Destroyed();
}

FTransform AMyCharacterAI::GetStartPosition() {
	return FTransform(Patrol_Start_Rotator, Patrol_Start_Location, FVector(1.0f, 1.0f, 1.0f));
}

FTransform AMyCharacterAI::GetNextPatrolPosition() {
	FVector vec = FVector(0, 0, 0);
	FRotator rot = FRotator(0, 0, 0);

	switch(Patrol_Type) {
	case E_EnemyPatrolType::Patrol_Spline:
		if (Patrol_Spline != nullptr)	{
			TTuple<FVector, int32, bool> tpl = Patrol_Spline->GetNextPosition(Patrol_Spline_Index, Patrol_Forward);
			vec = tpl.Get<0>();
			Patrol_Spline_Index = tpl.Get<1>();
			Patrol_Forward = tpl.Get<2>();
			float zrot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), vec).Yaw;
			rot = FRotator(0, zrot, 0);
		}
		else {
			vec = Patrol_Start_Location;
			rot = Patrol_Start_Rotator;
			Patrol_Type = E_EnemyPatrolType::Patrol_Stay;
		}
		break;
	case E_EnemyPatrolType::Patrol_Random: {
			TObjectPtr<UNavigationSystemV1> NavSystem = UNavigationSystemV1::GetNavigationSystem(this->GetWorld());
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

void AMyCharacterAI::LookAtPoint(FVector Point, float PlayRate) {
	if ((Point - GetActorLocation()).Length() < 25) {
		return;
	}
	
	TL_Look_Rotator_Begin = this->GetActorRotation();
	if (this->GetActorLocation() != Point) {
		TL_Look_Rotator_End = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Point);
	}
	else {
		TL_Look_Rotator_End = this->GetActorRotation();
	}
	
	TL_Look.PlayFromStart();
	TL_Look.SetPlayRate(PlayRate);
}

void AMyCharacterAI::ActiveDead() {
	Super::ActiveDead();
}

void AMyCharacterAI::TL_Look_UpdateFunc(float Value) {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("TL_Look_UpdateFunc(%f) : %f"), Value, TL_Look.GetPlaybackPosition()));
	
	float yaw = UKismetMathLibrary::REase(TL_Look_Rotator_Begin, TL_Look_Rotator_End, Value, true, EEasingFunc::CircularOut).Yaw;

	SetActorRotation(FRotator(0, yaw, 0), ETeleportType::None);
}

void AMyCharacterAI::TL_Look_FinishFunc() {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("TL_Look_FinishFunc")));
}

void AMyCharacterAI::ShowIcon(E_EnemyState State) {
	TObjectPtr<UWidget_AI_Reaction> tWidget = Cast<UWidget_AI_Reaction>(WidgetIcon->GetUserWidgetObject());
	tWidget->IconPlay(State);
}

void AMyCharacterAI::GetDamage(float Damage) {
	Super::GetDamage(Damage);
}

void AMyCharacterAI::GunFire_Implementation() {
	
}

void AMyCharacterAI::CheckCover() {
	FVector Start = this->GetActorLocation() - FVector(0.0f, 0.0f, 6.0f);
	FVector End = this->GetActorLocation() - FVector(0.0f, 0.0f, 6.0f) + this->GetActorForwardVector() * 200.0f;
	FVector Correct = FVector(0.0f, 0.0f, 100.0f);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	bool Result_Btm = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore,
	EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, GetWorld()->GetDeltaSeconds());

	bool Result_Top = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start + Correct, End + Correct,
	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore,
	EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, GetWorld()->GetDeltaSeconds());

	if (Result_Btm == true) {
		if (Result_Top == true) {
			AI_Controller->SetCurrentCover(E_EnemyCombatCoverType::Cover_WallHide);
		}
		else {
			AI_Controller->SetCurrentCover(E_EnemyCombatCoverType::Cover_Crouch);
		}
		return;
	}
	
	AI_Controller->SetCurrentCover(E_EnemyCombatCoverType::Cover_None);
}

void AMyCharacterAI::CheckOverlap() {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("CheckOverlap")));
	if ((AI_Controller->GetCurrentState() == E_EnemyState::E_StateDead) || (AI_Controller->GetCurrentState() == E_EnemyState::E_StateOff)) {
		return;
	}
	
	if (bDead == false) {
		if (AI_Controller->GetCurrentState() != E_EnemyState::E_StateAttack) {
			AI_Controller->RunSearchPointManager(Actor_Overlap->GetActorLocation());
			AI_Controller->StartAlert(Actor_Overlap->GetActorLocation());
		}
	}
}

void AMyCharacterAI::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("Overlap")));
	if (Cast<AMyCharacter>(OtherActor) == nullptr) {
		return;
	}

	Actor_Overlap = OtherActor;
	GetWorldTimerManager().SetTimer(Timer_Overlap, this, &AMyCharacterAI::CheckOverlap, 1.0f, false);
}
