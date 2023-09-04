// Fill out your copyright notice in the Description page of Project Settings.

#include "Characte_Ani.h"
#include "MyCharacter.h"

UCharacte_Ani::UCharacte_Ani()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Roll_Montage(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/Animations/Rolls/AS_RollToStand_Montage.AS_RollToStand_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Death_Montage(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/Animations/Deaths/AS_Death_01_Montage.AS_Death_01_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Valut_Montage(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Fence/AnimSeq_Traversal_Fence_91cm_JumpOver_Montage.AnimSeq_Traversal_Fence_91cm_JumpOver_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutThick_Montage(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Fence/AnimSeq_Traversal_Fence_91cm_JumpOver_Montage.AnimSeq_Traversal_Fence_91cm_JumpOver_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_End(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Climb/AnimSeq_Traversal_Climb_End_toCliffTop_Montage.AnimSeq_Traversal_Climb_End_toCliffTop_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_Start(TEXT("/Script/Engine.AnimMontage'/Game/WeaponSystem/Player/UsePlayerAnim/Retarget2/RE2_RE_HangStart.RE2_RE_HangStart'"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_Corner_Right(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Ledge/AnimSeq_Traversal_Ledge_Climb_InwardCorner_Right_Montage.AnimSeq_Traversal_Ledge_Climb_InwardCorner_Right_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_Corner_Left(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Ledge/ClimbCorner_Left_Montage.ClimbCorner_Left_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_InCorner_Right(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Ledge/AnimSeq_Traversal_Ledge_Climb_OutwardCorner_Right_Montage.AnimSeq_Traversal_Ledge_Climb_OutwardCorner_Right_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ValutClimb_InCorner_Left(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Ledge/AnimSeq_Traversal_Ledge_Climb_OutwardCorner_Left_Montage.AnimSeq_Traversal_Ledge_Climb_OutwardCorner_Left_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Hang_Start(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/UTA/AnimSeq/Traversal_Movement/HangStart.HangStart'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Amsal_Mon(TEXT("/Script/Engine.AnimMontage'/Game/Model/ProjectAnimation/StealthFinishers_KnifeAndHand/Animations/Player_Amsal.Player_Amsal'"));


	if (Roll_Montage.Succeeded())
		RollMontage = Roll_Montage.Object;

	if (Death_Montage.Succeeded())
		DeathMontage = Death_Montage.Object;

	if (Valut_Montage.Succeeded())
		ValutMontage = Valut_Montage.Object;

	if (ValutThick_Montage.Succeeded())
		ValutThickMontage = ValutThick_Montage.Object;

	if (ValutClimb_End.Succeeded())
		Climb_End = ValutClimb_End.Object;

	if (ValutClimb_Start.Succeeded())
		Climb_Start = ValutClimb_Start.Object;

	if (ValutClimb_Corner_Right.Succeeded())
		Climb_CornerRight = ValutClimb_Corner_Right.Object;

	if (ValutClimb_Corner_Left.Succeeded())
		Climb_CornerLeft = ValutClimb_Corner_Left.Object;

	if (ValutClimb_InCorner_Right.Succeeded())
		Climb_InCornerRight = ValutClimb_InCorner_Right.Object;

	if (ValutClimb_InCorner_Left.Succeeded())
		Climb_InCornerLeft = ValutClimb_InCorner_Left.Object;

	if (Hang_Start.Succeeded())
		HangStart = Hang_Start.Object;

	if (Amsal_Mon.Succeeded())
		AmsalMontage = Amsal_Mon.Object;

}

void UCharacte_Ani::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UCharacte_Ani::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	if (IsValid(MyPawn))
	{
		if (Player)
		{
			Is_Crouch = Player->Get_crocuhState();
			Is_Sprint = Player->Get_SprintState();
			Forward = Player->Get_Forward();
			Left = Player->Get_Left();
			Hide_Wall = Player->Get_WallHide();
			Get_Weapon = Player->Get_Weapons();
			Player_Yaw = Player->Get_Yaw();
			Player_Pitch = Player->Get_Pitch();
			LowHideWall = Player->Get_LowWallHide();
			IsHang = Player->Get_Hang();
			MoveDirection = Player->Get_Direction();
		}
	}
}

void UCharacte_Ani::PlayRollMontage()
{
	Montage_Play(RollMontage, 1.2f);
}

void UCharacte_Ani::PlayDeathMontage()
{
	Montage_Play(DeathMontage, 1.f);
}

void UCharacte_Ani::PlayValutMontage()
{
	Montage_Play(ValutMontage, 1.f);
}

void UCharacte_Ani::PlayValutThickMontage()
{
	Montage_Play(ValutThickMontage, 1.f);
}

void UCharacte_Ani::PlayClimbEnd()
{
	Montage_Play(Climb_End, 1.f);
}

void UCharacte_Ani::PlayClimbStart()
{
	Montage_Play(Climb_Start, 1.f);
}


void UCharacte_Ani::AnimNotify_OverWall()
{
	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	Player->Over_Wall();
	Montage_Stop(0.f, ValutMontage);
}

void UCharacte_Ani::AnimNotify_UpdateStart()
{
	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	Player->Move_Roll();
}


void UCharacte_Ani::AnimNotify_UdateEnd()
{
	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	Player->MoveEnd_Roll();
}

void UCharacte_Ani::AnimNotify_Climb()
{
	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	Player->ClimbEndNotify();
}

void UCharacte_Ani::PlayCornerRightMontage()
{
	Montage_Play(Climb_CornerRight, 1.f);
}

void UCharacte_Ani::PlayCornerLeftMontage()
{
	Montage_Play(Climb_CornerLeft, 1.f);
}

void UCharacte_Ani::PlayInCornerRightMontage()
{
	Montage_Play(Climb_InCornerRight, 1.f);
}

void UCharacte_Ani::PlayInCornerLeftMontage()
{
	Montage_Play(Climb_InCornerLeft, 1.f);
}

void UCharacte_Ani::PlayHangStartMontage()
{
	Montage_Play(HangStart, 1.f);
}

void UCharacte_Ani::PlayAmsalMontage()
{
	Montage_Play(AmsalMontage, 1.f);
}


void UCharacte_Ani::AnimNotify_Jump()
{
	auto MyPawn = TryGetPawnOwner();
	auto Player = Cast<AMyCharacter>(MyPawn);

	Player->Hang_Jump();
}