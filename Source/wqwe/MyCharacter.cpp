// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "test5/UIGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "MyCharacterAI_Controller.h"
#include "MyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Characte_Ani.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WeaponComponent.h"
#include "ADS_Camera.h"
#include "MyCharacterAI.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
	RootComponent = GetCapsuleComponent();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	GetCharacterMovement()->MaxFlySpeed = 150.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 500.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	LeftCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("LeftCamera"));
	LeftCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	LeftCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AI_POS = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AI_Pos"));
	AI_POS->SetupAttachment(RootComponent);
	AI_POS->SetWorldLocation(FVector(0,20.0f,-88.f));
	AI_POS->SetWorldRotation(FRotator(0, 0, -90.f));

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	//Head->SetupAttachment(GetMesh());
	Head->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("None"));

	Sprint_Speed = 600.f;
	Search_Distance = 30.f;
	Search_HeightDistance = 100.f;

	Search_LeftDistance = 100.f;
	RollDistance = 100.f;
	Search_RightEndDistance = 30.f;
	DeathTime = 0;

	WeaponSystem = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponSystem"));


}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	EndHang();
	EndHide();

	Speed = 300.f;
	
	TimeLineUpdatedelegate.BindUFunction(this, FName("TimeLineUpdateFunc"));
	timeLine.AddInterpFloat(TimeLineCurve, TimeLineUpdatedelegate);
	timeLine.SetTimelineLength(2.0f);

	TimeLineAimingUpdatedelegate.BindUFunction(this, FName("TimeLineAimingUpdateFunc"));
	timeLineAiming.AddInterpFloat(TimeLineAimingCurve, TimeLineAimingUpdatedelegate);
	timeLineAiming.SetTimelineLength(1.0f);

	TimeLineCrouchCapsuleUpdatedelegate.BindUFunction(this, FName("TimeLineCrouchUpdateFunc"));
	TimeLineCrouchCameraUpdatedelegate.BindUFunction(this, FName("TimeLineCrouchCameraUpdateFunc"));

	SmoothCrouchingCurveTimeLine.AddInterpFloat(TimeCrouchingCapsuleCurve, TimeLineCrouchCapsuleUpdatedelegate);
	SmoothCrouchingCurveTimeLine.AddInterpFloat(TimeCrouchingCameraCurve, TimeLineCrouchCameraUpdatedelegate);
	SmoothCrouchingCurveTimeLine.SetTimelineLength(0.3f);

	// 내가 추가한거
	// 현재 월드의 게임 모드를 받아온다
	AUIGameMode* GameMode = Cast<AUIGameMode>(GetWorld()->GetAuthGameMode());
	// 게임 모드에서 위젯을 받아온다
	MainHUDWidget = GameMode->GetMainHUDWidget();
	DamageHUDWidget = GameMode->GetDamageHUDWidget();
	CrossHairWidget = GameMode->GetCrossHairWidget();
	SniperWidget = GameMode->GetSniperWidget();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::TriggerEnter);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::TriggerExit);

	WeaponSystem->InitalizeWeaponComponent(FollowCamera, GunSlot[0], GunSlot[1], GunSlot[2], true);
}

void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Anim = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

	if (Anim != nullptr)
	{
		Anim->OnMontageEnded.AddDynamic(this, &AMyCharacter::EndMontaged);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeLine.TickTimeline(DeltaTime);
	timeLineAiming.TickTimeline(DeltaTime);
	SmoothCrouchingCurveTimeLine.TickTimeline(DeltaTime);

	Camera_Way = FollowCamera->GetComponentRotation();

	if (!Is_Valut || !IsHang)
	{
		Get_Trace();

		if(!Play_Amsal)
			CheckAmsal();
	}

	if (Play_Amsal)
		GetCharacterMovement()->DisableMovement();

	if (IsHang)
	{
		Check_Climb();
		CapsuleRight();
		CapsuleLeft();
	}

	if (Roll_Move)
	{
		FVector Loc1 = GetActorLocation();
		FVector Loc2 = Loc1 + GetActorForwardVector() * RollDistance;

		FVector Loc3 = UKismetMathLibrary::VInterpTo(Loc1, Loc2, GetWorld()->GetDeltaSeconds(), 1.f);

		SetActorLocation(Loc3);
	}

	srand(time(NULL));
	Time += DeltaTime;
	RandomNum = rand() % 2;

	FString bulletText;
	FString bulletAllText;
	FString bulletText2;
	FString bulletAllText2;
	FString bulletText3;
	FString bulletAllText3;
	/*bulletText = FString::Printf(TEXT("%d"), bullet);
	bulletAllText = FString::Printf(TEXT("%d"), bulletall);
	bulletText2 = FString::Printf(TEXT("%d"), bullet2);
	bulletAllText2 = FString::Printf(TEXT("%d"), bulletall2);
	bulletText3 = FString::Printf(TEXT("%d"), bullet3);
	bulletAllText3 = FString::Printf(TEXT("%d"), bulletall3);*/

	if (IsValid(MainHUDWidget))
	{
		if (WeaponSystem->IsDamage == false)
		{
			if (Time >= 5.0f)
			{
				HP += 1.0f;
				MainHUDWidget->SetHP(HP / HP_Max);
			}
		}
		else if (WeaponSystem->IsDamage == true)
			Time = 0.0f;

		//if (ChooseWeapon1)
		//{
		//	// 총알 갯수 설정한다
		//	MainHUDWidget->SetBullet(FText::FromString(bulletText));
		//	// 총알 전체 갯수 설정한다
		//	MainHUDWidget->SetBulletAll(FText::FromString(bulletAllText));
		//}
		//else if (ChooseWeapon2)
		//{
		//	// 총알 갯수 설정한다
		//	MainHUDWidget->SetBullet2(FText::FromString(bulletText2));
		//	// 총알 전체 갯수 설정한다
		//	MainHUDWidget->SetBulletAll2(FText::FromString(bulletAllText2));
		//}
		//else if (ChooseWeapon3)
		//{
		//	// 총알 갯수 설정한다
		//	MainHUDWidget->SetBullet3(FText::FromString(bulletText3));
		//	// 총알 전체 갯수 설정한다
		//	MainHUDWidget->SetBulletAll3(FText::FromString(bulletAllText3));
		//}
	}

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::LeftRight);

	InputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);
	InputComponent->BindAxis("LookTurn", this, &AMyCharacter::TurnLook);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &AMyCharacter::StartCrouch);
	InputComponent->BindAction("Roll", IE_Pressed, this, &AMyCharacter::Start_Roll);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AMyCharacter::Do_Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AMyCharacter::End_Sprint);

	InputComponent->BindAction("Valut", IE_Pressed, this, &AMyCharacter::StartValut);
	InputComponent->BindAction("CameraWibbing",IE_Pressed, this, &AMyCharacter::CameraWibbingFun);
	InputComponent->BindAction("Amsal", IE_Pressed, this, &AMyCharacter::StartAmsal);

	//준혁씨꺼
	InputComponent->BindAction("Aiming", IE_Pressed, this, &AMyCharacter::Aiming);
	InputComponent->BindAction("Aiming", IE_Released, this, &AMyCharacter::AimingEnd);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AMyCharacter::ReloadStart);

	InputComponent->BindAction("Switch Selecter", IE_Pressed, this, &AMyCharacter::ChangeSelector);

	InputComponent->BindAction("FPP Mode", IE_Pressed, this, &AMyCharacter::FPP_Mode);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::Fire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMyCharacter::FireRelease);

	//InputComponent->BindAction("Switch Selecter", IE_Pressed, this, &AMyCharacter::ChangeSelector);

	InputComponent->BindAction("Slot1", IE_Pressed, this, &AMyCharacter::ChangeSlot1);
	InputComponent->BindAction("Slot2", IE_Pressed, this, &AMyCharacter::ChangeSlot2);
	InputComponent->BindAction("Slot3", IE_Pressed, this, &AMyCharacter::ChangeSlot3);

	// 우원씨꺼
	/*PlayerInputComponent->BindAction(TEXT("HeadDamage"), EInputEvent::IE_Pressed, this, &AMyCharacter::HeadDamageSwitch);
	PlayerInputComponent->BindAction(TEXT("BodyDamage"), EInputEvent::IE_Pressed, this, &AMyCharacter::BodyDamageSwitch);
	PlayerInputComponent->BindAction(TEXT("ArmLegDamage"), EInputEvent::IE_Pressed, this, &AMyCharacter::ArmLegDamageSwitch);
	PlayerInputComponent->BindAction(TEXT("MinDamage"), EInputEvent::IE_Pressed, this, &AMyCharacter::MinDamageSwitch);
	//PlayerInputComponent->BindAction(TEXT("ResetHP"), EInputEvent::IE_Pressed, this, &AMyCharacter::ResetHPSwitch);
	//PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AMyCharacter::ShootSwitch);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AMyCharacter::ReloadStart);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartShoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &AMyCharacter::StopShoot);

	/*PlayerInputComponent->BindAction(TEXT("Weapon1Swap"), EInputEvent::IE_Pressed, this, &AMyCharacter::Weapon1SwapSwitch);
	PlayerInputComponent->BindAction(TEXT("Weapon2Swap"), EInputEvent::IE_Pressed, this, &AMyCharacter::Weapon2SwapSwitch);
	PlayerInputComponent->BindAction(TEXT("Weapon3Swap"), EInputEvent::IE_Pressed, this, &AMyCharacter::Weapon3SwapSwitch);*/

	PlayerInputComponent->BindAction(TEXT("WeaponSwap"), EInputEvent::IE_Pressed, this, &AMyCharacter::BackGround);
	//PlayerInputComponent->BindAction(TEXT("WeaponSwap"), EInputEvent::IE_Pressed, this, &AMyCharacter::BackGroundOnSwitch);
	PlayerInputComponent->BindAction(TEXT("WeaponSwap"), EInputEvent::IE_Released, this, &AMyCharacter::BackGroundOffSwitch);
}


void AMyCharacter::MoveForward(float Value)
{
	if (GetCharacterMovement()->IsFalling())
		return;

	if (Is_Roll || Is_Valut || Play_Amsal)
		return;

	Wall_Hide(Value);

	if (!pos_WallHide && !Low_Hide)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);

		Forward = Value;

		AddMovementInput(Direction, Value);
	}

	if (pos_WallHide || Low_Hide || IsHang)
	{
		Wall_HideMoveForWard(Value);
	}

	MoveDelay_Character();
}

void AMyCharacter::Wall_HideMoveForWard(float Value)
{
	if (Value < 0.f)
	{
		if (IsHang)
		{
			EndHang();
			return;
		}

		EndHide();
	}
}

void AMyCharacter::LeftRight(float Value)
{
	if (GetCharacterMovement()->IsFalling())
		return;

	if (Is_Roll || CornerAni || Play_Amsal)
		return;

	Left = Value;

	if (Value < 0)
		MoveDirection = true;
	else if(Value > 0)
		MoveDirection = false;

	if (!pos_WallHide && !Low_Hide && !IsHang)
	{
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}

	if (pos_WallHide || Low_Hide || IsHang)
	{
		FRotator Rot = UKismetMathLibrary::Conv_VectorToRotator(Wall_Normal * -1.f);
		FRotator Player_Rot = GetActorRotation();
		FRotator RRot = UKismetMathLibrary::RInterpTo(Player_Rot, Rot, GetWorld()->GetDeltaSeconds(), 7.0f);
		SetActorRotation(RRot);

		if (IsHang)
			HangMove(Value);

		Wall_HideLeftRight(Value, RRot);
	}

	MoveDelay_Character();
}

void AMyCharacter::HangMove(float Value)
{
	if (IsHang && !CheckCornerRightResult && !CheckCornerRight2Result && Value > 0 && !HangUp)
	{
		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());
		
		if (nullptr == AnimInstance)
			return;

		CornerAni = true;
		AnimInstance->PlayInCornerRightMontage();
	}
	else if (IsHang && !CheckCornerLeftResult && !CheckCornerLeft2Result && Value < 0 && !HangUp)
	{
		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

		if (nullptr == AnimInstance)
			return;

		CornerAni = true;
		AnimInstance->PlayInCornerLeftMontage();
	}

	else if (IsHang && CheckCornerRightResult && Value > 0 && !HangUp)
	{
		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

		if (nullptr == AnimInstance)
			return;

		CornerAni = true;
		AnimInstance->PlayCornerRightMontage();
	}
	else if (IsHang && CheckCornerLeftResult && Value < 0 && !HangUp)
	{
		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

		if (nullptr == AnimInstance)
			return;

		CornerAni = true;
		AnimInstance->PlayCornerLeftMontage();
	}
}

void AMyCharacter::Wall_HideLeftRight(float Value, FRotator Rot)
{
	Direction2 = UKismetMathLibrary::GetRightVector(Rot);

	SetActorRotation(Rot);
	AddMovementInput(Direction2, Value);
}

void AMyCharacter::LookUp(float Value)
{
	Aim_Pitch = Camera_Way.Pitch;
	AddControllerPitchInput(Value);
}

void AMyCharacter::TurnLook(float Value)
{
	OffSet_AIm();
	AddControllerYawInput(Value);
}

void AMyCharacter::Start_Roll()
{
	if (Is_Roll || pos_WallHide || Low_Hide || Is_Valut)
		return;

	if (IsHang && !CheckClimbResult && !HangUp)
	{
		FTimerHandle GravityTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(GravityTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				// 코드 구현
				IsHang = false;
				auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

				if (nullptr == AnimInstance)
					return;

				AnimInstance->PlayClimbEnd();
				HangUp = true;
				// TimerHandle 초기화
				GetWorld()->GetTimerManager().ClearTimer(GravityTimerHandle);
			}), 0.3f, false);
		return;
	}

	if (IsHang)
		return;

	TraceRollDistance();
	CheckHangWall();

	if (CheckHangResult && !IsHang)
	{
		if (Is_Zoom)
			AimingEnd();

		IsHang = true;
		Hang_Slove = false;

		GetTraceValut();
		GetTraceValutHeight();

		FRotator Rot = UKismetMathLibrary::Conv_VectorToRotator(Wall_Normal * -1.f);
		FRotator Player_Rot = GetActorRotation();
		FRotator RRot = UKismetMathLibrary::RInterpTo(Player_Rot, Rot, GetWorld()->GetDeltaSeconds(), 1.f);

		FVector Loc1 = ValutHeight - GetActorLocation();

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		TempEWeapon = WeaponSystem->CurrentGunSlot;

		WeaponSystem->CurrentGunSlot = EWeaponSlot::WeaponNone;
		Get_Weapon = 0;
		WeaponSystem->UpdateGunAttachment();

		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

		if (nullptr == AnimInstance)
			return;

		CornerAni = true;
		AnimInstance->PlayHangStartMontage();
		return;
	}

	if (!Is_Roll && !CheckRollDistanceResult)
	{
		if (GetCharacterMovement()->IsFalling())
			return;

		if (Is_Zoom)
			AimingEnd();

		Is_Roll = true;
		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());

		if (nullptr == AnimInstance)
			return;

		AnimInstance->PlayRollMontage();
		return;
	}
}

void AMyCharacter::StartCrouch()// 앉기
{
	if (Low_Hide || pos_WallHide || Is_Valut || IsHang || Play_Amsal)
		return;

	if (!Is_crouch)
	{
		if (Is_Spirnt)
		{
			Is_Spirnt = false;
		}
	
		Is_crouch = true;
		GetCharacterMovement()->MaxWalkSpeed = Crouch_Walkspeed;
		SmoothCrouchingCurveTimeLine.Play();
	}
	else
	{
		Is_crouch = false;
		GetCharacterMovement()->MaxWalkSpeed = Max_Walk;
		SmoothCrouchingCurveTimeLine.Reverse();
	}
}

void AMyCharacter::Aiming()
{
	if (Is_Spirnt)
	{
		End_Sprint();
	}

	if (!Is_Zoom && Get_Weapon != 0)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		timeLineAiming.Play();
		CrossHairWidget->SetVisibility(ESlateVisibility::Visible);
	}
	Is_Zoom = true;
	WeaponSystem->IsAiming = true;
}

void AMyCharacter::AimingEnd()
{
	if (Is_Zoom)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		timeLineAiming.Reverse();
		CrossHairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (WeaponSystem->ADSMode)
	{
		FPP_Mode();
	}

	Is_Zoom = false;
	WeaponSystem->IsAiming = false;
}

void AMyCharacter::StartAmsal()// 암살
{
	if (Low_Hide || pos_WallHide || Is_Valut || IsHang || Play_Amsal)
		return;
	
	if (IsAmsal) {
		if (IsValid(AiPawn) == true) {
			if (AiPawn->GetDead() == false) {
				if (Is_Zoom)
					AimingEnd();

				Is_crouch = false;
				UnCrouch();

				Play_Amsal = true;

				Amsal_BP();

				TempEWeapon = WeaponSystem->CurrentGunSlot;

				WeaponSystem->CurrentGunSlot = EWeaponSlot::WeaponNone;
				Get_Weapon = 0;
				WeaponSystem->UpdateGunAttachment();

				auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());
				if (nullptr == AnimInstance)
					return;

				AnimInstance->PlayAmsalMontage();
				//CameraBoom->bDoCollisionTest = false;
				OtherAI->GetAmsal(AiPawn, AI_POS->GetComponentLocation());
			}
		}
	}
	else
	{
		TArray<AActor*> OverlapedActors;
		GetCapsuleComponent()->GetOverlappingActors(OverlapedActors);

		UE_LOG(LogTemp, Log, TEXT("Input Interaction"));

		for (AActor* Actor : OverlapedActors)
		{
			IInteractInterface* Interface = Cast<IInteractInterface>(Actor);

			if (Interface)
			{
				Interface->OnInteract(this);
				break;
			}
		}
	}
}

void AMyCharacter::Do_Sprint() //기본Zoom
{
	if (Is_crouch || Low_Hide || pos_WallHide)
		return;

	if (Is_Zoom)
	{
		AimingEnd();
		Is_Zoom = false;
		this->bUseControllerRotationYaw = false;
		TempEWeapon = WeaponSystem->CurrentGunSlot;
	}

	//임시 주석 처리
	/*WeaponSystem->CurrentGunSlot = EWeaponSlot::WeaponNone;
	Get_Weapon = 0;
	WeaponSystem->UpdateGunAttachment();*/
	Is_Spirnt = true;
	Max_Speed = Sprint_Speed;
}

void AMyCharacter::End_Sprint() //기본Zoom
{
	//임시 주석
	/*WeaponSystem->CurrentGunSlot = TempEWeapon;
	WeaponSystem->UpdateGunAttachment();

	if (TempEWeapon == EWeaponSlot::WeaponSlot1)
	{
		Get_Weapon = 1;
	}
	else if (TempEWeapon == EWeaponSlot::WeaponSlot2)
	{
		Get_Weapon = 2;
	}
	else if (TempEWeapon == EWeaponSlot::WeaponSlot3)
	{
		Get_Weapon = 3;
	}*/
	Is_Spirnt = false;
	Speed = Max_Walk;
	GetCharacterMovement()->MaxWalkSpeed = Max_Walk;
}

void AMyCharacter::CameraWibbingFun()
{
	if (!ChangeCameraPos)
	{
		ChangeCameraPos = true;
		timeLine.Play();
	}
	else
	{
		ChangeCameraPos = false;
		timeLine.Reverse();
	}
}

void AMyCharacter::Get_Trace()
{
	Camera_Loc = GetActorLocation();
	CameraFor_Loc = GetActorForwardVector();

	GetNormal();
	GetPlayerUp();
}

void AMyCharacter::CheckHangWall()
{
	FVector Head_Start = GetActorLocation() + (GetActorUpVector() * 88.f);
	FVector Head_End = Head_Start + GetActorForwardVector() * Search_Distance;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	CheckHangResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Head_Start,
		Head_End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);
}

void AMyCharacter::CheckAmsal()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 50;
	FVector End = Start + GetActorForwardVector() * 10;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(WorldPawn);

	TArray<AActor*> TraceIgnoreActors;
	TraceIgnoreActors.Add(this);

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	CheckAmsalResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (CheckAmsalResult) {
		AiPawn = Cast<AMyCharacterAI>(HitResult.GetActor());

		if (IsValid(AiPawn) == false) {
			IsAmsal = false;
		}
		else {
			OtherAI = Cast<AMyCharacterAI_Controller>(AiPawn->GetController());

			if (AiPawn->GetDead() == false) {
				if (IsValid(OtherAI) == true) {
					if (!OtherAI->Perception_Sight) {
						IsAmsal = true;
					}
					else {
						IsAmsal = false;
					}
				}
			}
		}
	}
	else {
		IsAmsal = false;
	}
}

void AMyCharacter::GetNormal()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * Search_Distance;

	FVector Head_Start = GetActorLocation() + (GetActorUpVector() * 88.f);
	FVector Head_End = Head_Start + GetActorForwardVector() * Search_Distance;

	//2. In Corner 
	FVector CornerRight_Start = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * 33.f;
	FVector CornerRight_End = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * 66.f;

	FVector CornerLeft_Start = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * -33.f;
	FVector CornerLeft_End = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * -66.f;


	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WallHide = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WallHide);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes2;
	ObjectTypes2.Add(WorldStatic);
	ObjectTypes2.Add(WallHide);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.
	FHitResult HeadResult; // 히트 결과 값 받을 변수.
	FHitResult LeftResult; // 히트 결과 값 받을 변수.
	FHitResult RightResult; // 히트 결과 값 받을 변수.

	Wall_HeadTrace = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			Head_Start,
			Head_End,
			ObjectTypes2,
			false,
			TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
			EDrawDebugTrace::None,
			HeadResult,
			true);

	Wall_NormalResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (Low_Hide || pos_WallHide)
	{
		CheckCornerLeft2Result = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			CornerLeft_Start,
			CornerLeft_End,
			ObjectTypes,
			false,
			TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
			EDrawDebugTrace::None,
			LeftResult,
			true);

		CheckCornerRight2Result = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			CornerRight_Start,
			CornerRight_End,
			ObjectTypes,
			false,
			TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
			EDrawDebugTrace::None,
			RightResult,
			true);

	}

	if (Wall_NormalResult == true)
	{
		Wall_Normal = HitResult.Normal;
	}
	else
	{
		if (pos_WallHide || Low_Hide)
		{
			EndHide();
		}
	}
}

void AMyCharacter::OffSet_AIm()
{
	if (GetActorRotation().Yaw - Camera_Way.Yaw <= 100.f && GetActorRotation().Yaw - Camera_Way.Yaw >= -100.f && !flag)
		Aim_Yaw = UKismetMathLibrary::ClampAngle(GetActorRotation().Yaw - Camera_Way.Yaw, -100.f, 100.f);
	else
	{
		Aim_Yaw = 0;
	}
}

void AMyCharacter::MoveDelay_Character()
{
	if (Is_Spirnt )
	{
		if (Speed < Max_Speed)
		{
			Speed += 2;

			GetCharacterMovement()->MaxWalkSpeed = Speed;
		}
	}
}

void AMyCharacter::GetTraceValut()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 60.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	ValutTraceResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (ValutTraceResult)
	{
		ValutLocation = HitResult.Location;
		ValutNormal = HitResult.Normal;
	}
}

void AMyCharacter::GetTraceValutHeight()
{
	FVector End = ValutLocation + UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotFromX(ValutNormal)) * -10.f;
	FVector Start = End + FVector(0, 0, 900);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	ValutTraceHeightResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (ValutTraceHeightResult)
	{
		ValutHeight = HitResult.Location;

	}
}

void AMyCharacter::Check_Climb()
{
	FVector Start = GetActorLocation() + FVector(0, 0, 150.f) + GetActorForwardVector() * 45.f;
	FVector End = Start + FVector(0, 0, 130.f) + GetActorForwardVector() * 45.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	CheckClimbResult = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		20.f,
		20.f,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

}

void AMyCharacter::GetTraceValutThick()
{
	FVector End = ValutLocation + UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotFromX(ValutNormal)) * -70.f;
	FVector Start = End + FVector(0, 0, 900);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	ValutTraceThickResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (ValutTraceThickResult)
	{
		Walls_Thick = true;
	}
	else
		Walls_Thick = false;
}

void AMyCharacter::TraceRollDistance()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * RollDistance;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WallHide = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WallHide);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	CheckRollDistanceResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);
}

void AMyCharacter::StartValut()
{
	GetTraceValut();

	if (GetCharacterMovement()->IsFalling())
		return;

	if (Is_Valut || IsHang || !ValutTraceResult)
		return;

	if (pos_WallHide || Low_Hide)
		Wall_HideMoveForWard(-1.f);

	if (ValutTraceResult)
	{
		GetTraceValutHeight();
		GetTraceValutThick();

		auto AnimInstance = Cast<UCharacte_Ani>(GetMesh()->GetAnimInstance());
		Is_Valut = true;

		FVector Loc1 = ValutHeight - GetActorLocation();
		CameraBoom->bDoCollisionTest = false;

		if (Loc1.Z >= 210.f)
		{
			Is_Valut = false;
			CameraBoom->bDoCollisionTest = true;
			return;
		}

		if (Loc1.Z >= 80.f)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

			if (Loc1.Z >= 180.f)
			{
				AnimInstance->PlayClimbStart();

				GetMesh()->SetWorldLocation(GetActorLocation() + GetActorForwardVector() * 20.f - GetActorUpVector() * 88.f);
			}
			else if (Loc1.Z <= 160.f)
			{
				AnimInstance->PlayClimbEnd();
			}
			else
			{
				Is_Valut = false;
				CameraBoom->bDoCollisionTest = true;
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				return;
			}
		}
		else if (Walls_Thick)
		{
			Is_Valut = false;
			CameraBoom->bDoCollisionTest = true;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			return;
		}
		else
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

			SetActorLocation(GetActorLocation());
			AnimInstance->PlayValutMontage();
		}
	}
}

void AMyCharacter::EndMontaged(UAnimMontage* Montage, bool bInterrupted)
{
	if (Is_Valut)
	{
		Is_Valut = false;
		CameraBoom->bDoCollisionTest = true;
	}

	if (Is_Roll)
	{
		Is_Roll = false;
		Roll_Move = false;

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (CornerAni)
		CornerAni = false;

	if (HangUp)
	{
		HangUp = false;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EndHang();
	}

	if (HP <= 0)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AActor::Destroy();
	}

	if (Play_Amsal)
	{
		Play_Amsal = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		WeaponSystem->CurrentGunSlot = TempEWeapon;
		WeaponSystem->UpdateGunAttachment();
		CameraBoom->bDoCollisionTest = true;

		if (TempEWeapon == EWeaponSlot::WeaponSlot1)
		{
			Get_Weapon = 1;
		}
		else if (TempEWeapon == EWeaponSlot::WeaponSlot2)
		{
			Get_Weapon = 2;
		}
		else if (TempEWeapon == EWeaponSlot::WeaponSlot3)
		{
			Get_Weapon = 3;
		}
	}
}

void AMyCharacter::Over_Wall()
{
	FVector Loc1 = GetActorLocation();
	FVector Loc2 = Loc1 - FVector(0, 0, 50.f);

	FVector Loc3 = UKismetMathLibrary::VInterpTo(Loc1, Loc2, GetWorld()->GetDeltaSeconds(), 1.f);

	SetActorLocation(Loc3);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

}

void AMyCharacter::ClimbEndNotify()
{
	if (Is_Valut || HangUp)
	{
		GetMesh()->SetWorldLocation(GetActorLocation() + FVector(0, 0, -88.f));
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void AMyCharacter::Hang_Jump()
{
	FVector Loc1 = ValutHeight - GetActorLocation();

	if (Loc1.Z >= Player_Up.Z && CheckUpWallResult)
	{
		Loc1 = Player_Up;
	}
	FVector Loc2 = GetActorLocation() + GetActorUpVector() * (Loc1.Z * 0.5 - PlayerHeight);

	GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	SetActorLocation(Loc2);
}

void AMyCharacter::Move_Roll()
{
	Roll_Move = true;
}

void AMyCharacter::MoveEnd_Roll()
{
	Roll_Move = false;
}

void AMyCharacter::Wall_Hide(float Value)
{
	if (Is_Valut || IsHang || Is_Roll || HangUp)
		return;

	if (Wall_NormalResult && !Wall_HeadTrace && Value > 0
		|| Wall_HeadTrace && Wall_NormalResult && Value > 0)
	{
		HideTime += 2;

		if (Value >= 0)
			Value = 0;
	}

	if ((Wall_NormalResult && !Wall_HeadTrace && HideTime >= CheckHideTime) ||
		Wall_HeadTrace && Wall_NormalResult && HideTime >= CheckHideTime && Is_crouch)
	{
		HideTime = 0;
		Low_Hide = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCapsuleComponent()->SetCapsuleHalfHeight(50.f);
		GetMesh()->SetWorldLocation(GetActorLocation() + FVector(0, 0, -48.f));

		Is_crouch = false;
		UnCrouch();
		Is_Spirnt = false;
	}
	else if (Wall_HeadTrace && Wall_NormalResult && HideTime >= CheckHideTime && !Is_crouch)
	{
		HideTime = 0;
		pos_WallHide = true;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		Is_crouch = false;
		UnCrouch();
		Is_Spirnt = false;
	}
	else
	{
		return;
	}
}

void AMyCharacter::GetPlayerUp()
{
	//1. Out Corner
	FVector Start = GetActorLocation();
		
	FVector End = GetActorLocation()+ GetActorUpVector() * 250.f;;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	CheckUpWallResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (CheckUpWallResult)
	{
		Player_Up = HitResult.Location;
	}
}

void AMyCharacter::CapsuleRight()
{
	//1. Out Corner
	FVector Start = GetActorLocation() + GetActorForwardVector() * -44.f + GetActorRightVector() * 66.f
		+ GetActorUpVector() * 25.f;
	FVector End = GetActorLocation() + GetActorForwardVector() * -44.f + GetActorRightVector() * 66.f
		+ GetActorUpVector() * -25.f;;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	//2. In Corner 
	FVector Start2 = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * 33.f;
	FVector End2 = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * 66.f;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult2; // 히트 결과 값 받을 변수.

	CheckCornerRightResult = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		34.f,
		55.f,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	CheckCornerRight2Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start2,
		End2,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult2,
		true);
}

void AMyCharacter::CapsuleLeft()
{
	//1. Out Corner
	FVector Start = GetActorLocation() + GetActorForwardVector() * -44.f + GetActorRightVector() * -66.f
		+ GetActorUpVector() * 25.f;
	FVector End = GetActorLocation() + GetActorForwardVector() * -44.f + GetActorRightVector() * -66.f
		+ GetActorUpVector() * -25.f;;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	ObjectTypes.Add(WorldStatic);

	TArray<AActor*> TraceIgnoreActors;

	TArray<UPrimitiveComponent*> OutComponents;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	//2. In Corner 
	FVector Start2 = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * -33.f;
	FVector End2 = GetActorLocation() + GetActorForwardVector() * 44.f + GetActorRightVector() * -66.f;

	// 히트 결과 값 받을 변수.
	FHitResult HitResult2; // 히트 결과 값 받을 변수.

	CheckCornerLeftResult = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		34.f,
		55.f,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult,
		true);

	CheckCornerLeft2Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		Start2,
		End2,
		ObjectTypes,
		false,
		TraceIgnoreActors, // 무시할 것이 없다고해도 null을 넣을 수 없다.
		EDrawDebugTrace::None,
		HitResult2,
		true);
}

void AMyCharacter::EndHang()
{
	if (!CornerAni || IsHang)
	{
		IsHang = false;
		Hang_Slove = true;
		GetMesh()->SetWorldLocation(GetActorLocation() + FVector(0, 0, -88.f));
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		if (TempEWeapon != EWeaponSlot::WeaponNone)
		{
			WeaponSystem->CurrentGunSlot = TempEWeapon;
			WeaponSystem->UpdateGunAttachment();

			if (TempEWeapon == EWeaponSlot::WeaponSlot1)
			{
				Get_Weapon = 1;
			}
			else if (TempEWeapon == EWeaponSlot::WeaponSlot2)
			{
				Get_Weapon = 2;
			}
			else if (TempEWeapon == EWeaponSlot::WeaponSlot3)
			{
				Get_Weapon = 3;
			}
		}
	}
}

void AMyCharacter::EndHide()
{
	pos_WallHide = false;
	Low_Hide = false;
	HideTime = 0;
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);
	GetMesh()->SetWorldLocation(GetActorLocation() + FVector(0, 0, -88.f));
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

// 내가 추가한거

/*void AMyCharacter::HeadDamageSwitch()
{
	opval = 0.5f;
	//DamageHUDWidget->SetOp(opval);

	hpval -= 1.0f;
	isDamage = true;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::StopDamage, .1f, false);
}

void AMyCharacter::BodyDamageSwitch()
{
	opval += 0.15f;
	if (opval > 0.5f)
		opval = 0.5f;
	//DamageHUDWidget->SetOp(opval);

	hpval -= 0.3f;
	isDamage = true;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::StopDamage, .1f, false);
}

void AMyCharacter::ArmLegDamageSwitch()
{
	opval += 0.05f;
	if (opval > 0.5f)
		opval = 0.5f;
	//DamageHUDWidget->SetOp(opval);

	hpval -= 0.1f;
	isDamage = true;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::StopDamage, .1f, false);
}

void AMyCharacter::MinDamageSwitch()
{
	opval += 0.015f;
	if (opval > 0.5f)
		opval = 0.5f;
	//DamageHUDWidget->SetOp(opval);

	hpval -= 0.03f;
	isDamage = true;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::StopDamage, .1f, false);
}

void AMyCharacter::ResetHPSwitch()
{
	opval = 0.0f;
	//DamageHUDWidget->SetOp(opval);

	hpval = 1.0f;
}*/

//void AMyCharacter::ShootSwitch()
//{
//	if (ChooseWeapon1)
//	{
//		if (isFiring)
//		{
//			bullet -= 1;
//			if (bullet <= 0)
//				bullet = 0;
//			GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::ShootSwitch, .1f, false);
//		}
//	}
//	else if (ChooseWeapon2)
//	{
//		if (isFiring)
//		{
//			bullet2 -= 1;
//			if (bullet2 <= 0)
//				bullet2 = 0;
//			GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::ShootSwitch, .1f, false);
//		}
//	}
//	else if (ChooseWeapon3)
//	{
//		if (isFiring)
//		{
//			bullet3 -= 1;
//			if (bullet3 <= 0)
//				bullet3 = 0;
//			GetWorld()->GetTimerManager().SetTimer(timer, this, &AMyCharacter::ShootSwitch, .1f, false);
//		}
//	}
//}
//
//void AMyCharacter::ReloadSwitch()
//{
//	if (ChooseWeapon1)
//	{
//		bulletgap = bulletdefault - bullet;
//
//		if (bulletall >= bulletgap)
//		{
//			bullet += bulletgap;
//			bulletall -= bulletgap;
//		}
//		else
//		{
//			bullet += bulletall;
//			bulletall = 0;
//		}
//
//		bulletgap = 0;
//	}
//
//	else if (ChooseWeapon2)
//	{
//		bulletgap2 = bulletdefault2 - bullet2;
//
//		if (bulletall2 >= bulletgap2)
//		{
//			bullet2 += bulletgap2;
//			bulletall2 -= bulletgap2;
//		}
//		else
//		{
//			bullet2 += bulletall2;
//			bulletall2 = 0;
//		}
//
//		bulletgap2 = 0;
//	}
//
//
//	else if (ChooseWeapon3)
//	{
//		bulletgap3 = bulletdefault3 - bullet3;
//
//		if (bulletall3 >= bulletgap3)
//		{
//			bullet3 += bulletgap3;
//			bulletall3 -= bulletgap3;
//		}
//		else
//		{
//			bullet3 += bulletall3;
//			bulletall3 = 0;
//		}
//
//		bulletgap3 = 0;
//	}
//}

void AMyCharacter::StartShoot()
{
	isFiring = true;
	//ShootSwitch();
}

void AMyCharacter::StopShoot()
{
	isFiring = false;
}

void AMyCharacter::StopDamage()
{
	isDamage = false;
}

//void AMyCharacter::Weapon1SwapSwitch()
//{
//	ChooseWeapon1 = true;
//	ChooseWeapon2 = false;
//	ChooseWeapon3 = false;
//	MainHUDWidget->SetWeapon1();
//}
//
//void AMyCharacter::Weapon2SwapSwitch()
//{
//	ChooseWeapon1 = false;
//	ChooseWeapon2 = true;
//	ChooseWeapon3 = false;
//	MainHUDWidget->SetWeapon2();
//}
//
//void AMyCharacter::Weapon3SwapSwitch()
//{
//	ChooseWeapon1 = false;
//	ChooseWeapon2 = false;
//	ChooseWeapon3 = true;
//	MainHUDWidget->SetWeapon3();
//}

//void AMyCharacter::BackGroundOnSwitch()
//{
//	isSwap = true;
//	BackGround();
//}
//
void AMyCharacter::BackGroundOffSwitch()
{
	MainHUDWidget->BackGroundOffVisi();
}

void AMyCharacter::BackGround()
{
	//MainHUDWidget->BackGroundVisi();
	WeaponSystem->SwapTexture();
	//GetWorld()->GetTimerManager().SetTimer(timer, this, &ATestUI2Character::BackGround, .1f, false);
}

void AMyCharacter::TimeLineUpdateFunc(float Output)
{
	FVector Loc1 = LeftCamera->GetComponentLocation();
	FVector Loc2 = GetActorRightVector() * Output;
	FVector Loc3 = GetActorUpVector()* 80.f;

	FollowCamera->SetWorldLocation(Loc1 + Loc2 + Loc3);
}

void AMyCharacter::TimeLineCrouchUpdateFunc(float Capsule)
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(Capsule);
}

void AMyCharacter::TimeLineCrouchCameraUpdateFunc(float Camera)
{
	FollowCamera->SetRelativeLocation(FVector(0.0f,80.0f, Camera));
}

void AMyCharacter::TimeLineAimingUpdateFunc(float Output)
{
	float RightDistance = 80.f;

	if (ChangeCameraPos)
		RightDistance = RightDistance * -1;

	CameraBoom->TargetArmLength = Output;
}

void AMyCharacter::OnInteraction()
{
	
}

void AMyCharacter::Fire()
{
	if (Is_Valut || IsHang || Is_Roll)
		return;

	if(Is_Zoom)
		WeaponSystem->GunFire();
}

void AMyCharacter::FireRelease()
{
	WeaponSystem->ReleaseFire();
}

void AMyCharacter::ChangeSelector()
{
	WeaponSystem->WeaponSelectorChange();
}

void AMyCharacter::ChangeSlot1()
{
	if (WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot1)] && (WeaponSystem->CurrentGunSlot != EWeaponSlot::WeaponSlot1 || !IsValid(WeaponSystem->CurrentEquipWeapon)))
	{
		PlaySlotReleaseAnimation(EWeaponSlot::WeaponSlot1);
		/*	WeaponSystem->EquipWeapon(EWeaponSlot::WeaponNone);

			PlaySlotHoldAnimation(EWeaponSlot::WeaponSlot1);
			WeaponSystem->EquipWeapon(EWeaponSlot::WeaponSlot1);*/

		Get_Weapon = 1;
	}
	else if (WeaponSystem->CurrentGunSlot == EWeaponSlot::WeaponSlot1 || !IsValid(WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot1)]))
	{
		PlaySlotHoldAnimation(EWeaponSlot::WeaponSlot1);
		Get_Weapon = 0;
	}
}
void AMyCharacter::ChangeSlot2()
{
	if (WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot2)] && (WeaponSystem->CurrentGunSlot != EWeaponSlot::WeaponSlot2 || !IsValid(WeaponSystem->CurrentEquipWeapon)))
	{
		PlaySlotReleaseAnimation(EWeaponSlot::WeaponSlot2);
		/*WeaponSystem->EquipWeapon(EWeaponSlot::WeaponSlot2);
		PlaySlotReleaseAnimation(EWeaponSlot::WeaponSlot2);*/
		Get_Weapon = 2;
	}
	else if (WeaponSystem->CurrentGunSlot == EWeaponSlot::WeaponSlot2 || !IsValid(WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot2)]))
	{
		PlaySlotHoldAnimation(EWeaponSlot::WeaponSlot2);
		Get_Weapon = 0;
	}

}
void AMyCharacter::ChangeSlot3()
{
	if (WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot3)] && (WeaponSystem->CurrentGunSlot != EWeaponSlot::WeaponSlot3 || !IsValid(WeaponSystem->CurrentEquipWeapon)))
	{
		PlaySlotReleaseAnimation(EWeaponSlot::WeaponSlot3);
		Get_Weapon = 3;
	}
	else if (WeaponSystem->CurrentGunSlot == EWeaponSlot::WeaponSlot3 || !IsValid(WeaponSystem->GunSlot[static_cast<int>(EWeaponSlot::WeaponSlot3)]))
	{
		PlaySlotHoldAnimation(EWeaponSlot::WeaponSlot3);
		Get_Weapon = 0;
	}

}

void AMyCharacter::ReloadStart()
{
	WeaponSystem->WeaponReload();
}

void AMyCharacter::PlayReloadAnimation()
{
	K2_PlayReloadAnimation();
}

void AMyCharacter::PlayFireAnimation()
{
	K2_PlayFireAnimation();
}

void AMyCharacter::PlaySlotHoldAnimation(EWeaponSlot _Slot)
{
	K2_PlaySlotHoldAnimation(_Slot);
}

void AMyCharacter::PlaySlotReleaseAnimation(EWeaponSlot _Slot)
{
	K2_PlaySlotReleaseAnimation(_Slot);
}

void AMyCharacter::TriggerEnter(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractInterface* Interface = Cast<IInteractInterface>(OtherActor);

	if (Interface)
	{
		Interface->OnInteractRangeEntered();
	}
}

void AMyCharacter::TriggerExit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractInterface* Interface = Cast<IInteractInterface>(OtherActor);

	if (Interface)
	{
		Interface->OnInteractRangeExited();
	}
}

void AMyCharacter::FPP_Mode()
{
	AGunBase* CurrentWeapon = WeaponSystem->CurrentEquipWeapon;
	bool ADSMode = WeaponSystem->ADSMode;
	float BlendTime = 0.4f;
	float BlendExp = 0.4f;

	if (Is_Zoom==false) return;

	if (IsValid(CurrentWeapon))
	{
		EWeaponType WeaponType = CurrentWeapon->WeaponType;
		AADS_Camera* ADS_Camera = CurrentWeapon->GetAdsCamera();
		
		if (ADSMode == false)
		{
			WeaponSystem->ADSMode = true;

			CurrentWeapon->UpdateAdsCameraPosition();
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(ADS_Camera, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic, BlendExp);
			WeaponSystem->SetAimPointCamera(ADS_Camera->GetCamera());

			CrossHairWidget->SetVisibility(ESlateVisibility::Collapsed);

			FTimerHandle TH;
			float InDelayTime = BlendTime - 0.1f;
			GetWorld()->GetTimerManager().SetTimer(TH, FTimerDelegate::CreateLambda([&]()
				{
					Head->SetVisibility(false);

					GetWorld()->GetTimerManager().ClearTimer(TH);
				}
			), InDelayTime, false);



			if (WeaponType == EWeaponType::SniperRifle)
			{
				SniperWidget->SetVisibility(ESlateVisibility::Visible);
				GetMesh()->SetVisibility(false);
				CurrentWeapon->SkeletalMesh->SetVisibility(false);
			}
		}
		else
		{
			WeaponSystem->ADSMode = false;

			LeftCamera->SetActive(false);
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic, BlendExp);
			LeftCamera->SetActive(true);

			Head->SetVisibility(true);
			CrossHairWidget->SetVisibility(ESlateVisibility::Visible);

			WeaponSystem->SetAimPointCamera(FollowCamera);

			if (WeaponType == EWeaponType::SniperRifle)
			{
				SniperWidget->SetVisibility(ESlateVisibility::Collapsed);
				GetMesh()->SetVisibility(true);
				CurrentWeapon->SkeletalMesh->SetVisibility(true);
			}
		}


	}
}
