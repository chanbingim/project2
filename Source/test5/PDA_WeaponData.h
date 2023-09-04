// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "ShellEjectionBase.h"
#include "MyCustomEnums.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PDA_WeaponData.generated.h"


/**
 * 
 */
UCLASS()
class TEST5_API UPDA_WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPDA_WeaponData();

	UPROPERTY(EditAnywhere, Category = "GunValue")
	FName WeaponName;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EWeaponSlot WeaponSlot;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	int AmmoIndex;
	
	UPROPERTY(EditAnywhere, Category = "GunValue")
	int MaxClip;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float SoundRadius;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	USoundBase* GunReloadSound;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UAnimationAsset* GunReloadAnim;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	USoundBase* GunFireSound;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UAnimationAsset* GunFireAnim;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UNiagaraSystem* GunFireParticle;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UTexture2D* WeaponHUD;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	TSubclassOf<AShellEjectionBase> ShellEjectionObj;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	TSubclassOf<AActor> MagazineObj;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	FVector2D WeaponSpread;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	FVector2D WeaponDamage;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UCurveFloat* GunRecoilCurveYaw;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UCurveFloat* GunRecoilCurvePitch;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float GunRecoilTimelineLength;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UNiagaraSystem* TrailParticle;

public:
	UPROPERTY(EditAnywhere, Category = "CharValue")
	UAnimationAsset* CharacterFireAnim;

	UPROPERTY(EditAnywhere, Category = "CharValue")
	UAnimationAsset* CharacterReloadAnim;
};
