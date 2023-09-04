// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characte_Ani.generated.h"

UCLASS()
class WQWE_API UCharacte_Ani : public UAnimInstance
{
	GENERATED_BODY()

	UCharacte_Ani();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Is_Crouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Is_Sprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	float Forward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	float Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Hide_Wall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	bool LowHideWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	int Get_Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	float Player_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	float Player_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	bool MoveDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	bool IsHang;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Roll", Meta = (AllowPrivateAccess = true))
		UAnimMontage* RollMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Death", Meta = (AllowPrivateAccess = true))
		UAnimMontage* DeathMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ValutMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* AmsalMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ValutThickMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_End;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_Start;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_CornerRight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_CornerLeft;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_InCornerRight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* Climb_InCornerLeft;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Valut", Meta = (AllowPrivateAccess = true))
		UAnimMontage* HangStart;
	
public:
	void PlayRollMontage();

	void PlayAmsalMontage();

	void PlayDeathMontage();

	void PlayValutMontage();

	void PlayCornerRightMontage();

	void PlayCornerLeftMontage();

	void PlayInCornerRightMontage();

	void PlayInCornerLeftMontage();

	void PlayValutThickMontage();

	void PlayHangStartMontage();

	void PlayClimbEnd();

	void PlayClimbStart();

	UFUNCTION()
	void AnimNotify_Climb();

	UFUNCTION()
	void AnimNotify_OverWall();

	UFUNCTION()
	void AnimNotify_UpdateStart();

	UFUNCTION()
	void AnimNotify_UdateEnd();

	UFUNCTION()
	void AnimNotify_Jump();
};
