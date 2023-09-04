// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character_Base.generated.h"

UCLASS()
class PROJECT_API ACharacter_Base : public ACharacter {
	GENERATED_BODY()
	
public:
	ACharacter_Base();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TArray<class AWeapon_Base*> Weapon_Array;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Weapon")
	AWeapon_Base* Weapon_Current;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 Weapon_Array_Max = 4;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 Weapon_Array_Index = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character")
	float Health = 100.0f;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void OnResetVR();
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void MoveForward(float Value);
	void MoveRight(float Value);

public:
	virtual void Tick(float DeltaTime) override;

	void GetWeapon(class AWeapon_Base* Weapon);
	void ReleaseWeapon();
	void FireWeapon(float Value);
	void ReloadWeapon();
	void ChangeWeapon(int32 Index);
	void ChangeWeapon_Up();
	void ChangeWeapon_Down();
	
	UFUNCTION(BlueprintCallable)
	bool IsDead();
};
