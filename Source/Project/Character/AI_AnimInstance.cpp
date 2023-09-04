// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_AnimInstance.h"
#include "Character_Base.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UAI_AnimInstance::UAI_AnimInstance() {
}

void UAI_AnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacter_Base>(TryGetPawnOwner());
}

void UAI_AnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character || DeltaSeconds == 0.0f) {
		return;
	}

	FVector NewVelocity = Character->GetVelocity();
	Speed = NewVelocity.Size();
	Direction = UKismetAnimationLibrary::CalculateDirection(NewVelocity, Character->GetActorRotation());

	SetCharacterProperties();
	if (bIsInCombat) {
		SetPitch();
		SetRootYawOffset();
		StartTurn = UKismetMathLibrary::Abs(RootYawOffset) > MaxTurnAngle;
		Recoil(DeltaSeconds);
		return;
	}
	RootYawOffset = 0.f;
}

void UAI_AnimInstance::SetCharacterProperties() {
	// bADS = Character->AnimValues.bADS;
	// bIsCrouching = Character->AnimValues.bIsCrouching;
	// bIsInCombat = Character->AnimValues.bIsInCombat;
	// bIsShooting = Character->AnimValues.bIsShooting;
	// bSitting = Character->AnimValues.bIsSitting;
	YawActual = RootYawOffset * -1.f;
	bSprinting = SetSprinting();
}

void UAI_AnimInstance::SetPitch() {
	FRotator PawnRotation = Character->GetActorRotation();
	FRotator AimRotation = Character->GetBaseAimRotation();
	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, PawnRotation).Pitch;
}

void UAI_AnimInstance::SetRootYawOffset() {
	if (Speed > 0.f || IsAnyMontagePlaying()) {
		RootYawOffset = 0.f;
	}

	YawLastTick = Yaw;
	Yaw = Character->GetActorRotation().Yaw;
	YawChangeOverFrame = YawLastTick - Yaw;

	RootYawOffset = UKismetMathLibrary::NormalizeAxis(YawChangeOverFrame + RootYawOffset);

	if (GetCurveValue(Turning) > 0.f) {
		DistanceCurveValueLastFrame = DistanceCurveValue;
		DistanceCurveValue = GetCurveValue(DistanceToPivot);

		(RootYawOffset > 0.f) ? TurnDirection = -1.f : TurnDirection = 1.f;
		DistanceCurveDifference = DistanceCurveValueLastFrame - DistanceCurveValue;

		RootYawOffset = RootYawOffset - (DistanceCurveDifference * TurnDirection);
		ABSRootYawOffset = UKismetMathLibrary::Abs(RootYawOffset);
		
		if (ABSRootYawOffset > MaxTurnAngle) {
			YawToSubtract = ABSRootYawOffset - MaxTurnAngle;
			YawMultiplier = 0.f;
			(RootYawOffset > 0.f) ? YawMultiplier = 1.f : YawMultiplier = -1.f;
			YawToSubtract = YawToSubtract * YawMultiplier;

			RootYawOffset = RootYawOffset - YawToSubtract;

		}
	}
	return;
}

void UAI_AnimInstance::Recoil(float Seconds) {
	if (!bIsShooting) {
		HandRotation = FRotator(0.f, 0.f, 0.f);
		return;
	}

	FRotator NewRotation;
	(HandRotation == FRotator(0.f, 0.f, 0.f)) ? NewRotation = FRotator(0.f, 0.f, -3.f) : NewRotation = FRotator(0.f, 0.f, 0.f);
	HandRotation = UKismetMathLibrary::RInterpTo(HandRotation, NewRotation, Seconds * 2.f, 15.f);
}

bool UAI_AnimInstance::SetSprinting() {
	return (Speed > 300.f);
}
