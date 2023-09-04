// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "MyCustomEnums.h"
#include "MySequenceNodes.h"
#include "Components/TimeLineComponent.h"
#include "PDA_WeaponData.h"
#include "PDA_GunSocketData.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GunBase.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API AGunBase : public AItemBase
{
	GENERATED_BODY()

public:
	AGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class UChildActorComponent* ADSCameraSystem;

public:
	UPROPERTY(EditAnywhere, Category = "CheckValue")
	bool CheckShoot;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EGunState GunState;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float SoundRadius;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EWeaponSlot WeaponSlot;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UTexture2D* WeaponHUD;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	int MaxClip;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	int CurrClip;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	int AmmoIndex;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float DefaultCameraFOV;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	bool AimingWithScope;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UPDA_WeaponData* WeaponData;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UPDA_GunSocketData* SocketData;

public:
	UPROPERTY(EditAnywhere, Category = "GunReload")
	class USoundBase* GunReloadSound;

	UPROPERTY(EditAnywhere, Category = "GunReload")
	class UAnimationAsset* GunReloadAnim;

public:
	UPROPERTY(EditAnywhere, Category = "GunFireFX")
	class USoundBase* GunFireSound;

	UPROPERTY(EditAnywhere, Category = "GunFireFX")
	class UAnimationAsset* GunFireAnim;

	UPROPERTY(EditAnywhere, Category = "GunFireFX")
	class UNiagaraSystem* GunFireParticle;

public:
	UPROPERTY(EditAnywhere, Category = "GunShellEject")
	TSubclassOf<class AShellEjectionBase> ShellEjectionObj;

public:
	//cpp에서 특정 클래스를 할당하는 방법
	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName MuzzleFlashName;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName SupressorMuzzleFlashName;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName AimingCameraLocation;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName AttachmentSocketLaser;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName AttachmentSocketSupressor;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName AttachmentSocketScope;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName BulletShellSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName HandGripTransform;

	UPROPERTY(EditAnywhere, Category = "GunSocket")
	FName Magazine;

public:
	//cpp에서 특정 클래스를 할당하는 방법
	UPROPERTY(EditAnywhere, Category = "GunMagazine")
	TSubclassOf<AActor> MagazineObj;

	UPROPERTY(EditAnywhere, Category = "GunMagazine")
	FVector MagazineVelocity;

public:
	UPROPERTY(EditAnywhere, Category = "GunRecoil")
	float RecoilRate;

	UPROPERTY(EditAnywhere, Category = "GunRecoil")
	float RecoilMaxThresHold;

	UPROPERTY(EditAnywhere, Category = "GunRecoil")
	FRotator OriginRotation;

	UPROPERTY(EditAnywhere, Category = "GunRecoil")
	FRotator PostRecoilRotation;

public:
	UPROPERTY(EditAnywhere, Category = "WeaponScale")
	float WeaponMinSpread;

	UPROPERTY(EditAnywhere, Category = "WeaponScale")
	float WeaponMaxSpread;

	UPROPERTY(EditAnywhere, Category = "WeaponScale")
	float WeaponMinDamage;

	UPROPERTY(EditAnywhere, Category = "WeaponScale")
	float WeaponMaxDamage;

public:
	UPROPERTY(EditAnywhere, Category = "Attachments")
	TArray<class AActor*> Attachments;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	bool OnSpawn_Optic;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	bool OnSpawn_Laser;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	bool OnSpawn_Supressor;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	TSubclassOf<class AAttachmentBase> OpticsClass;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	TSubclassOf<class AAttachmentBase> LaserClass;

	UPROPERTY(EditAnywhere, Category = "Attachments")
	TSubclassOf<class AAttachmentBase> SupressorClass;

public:
	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* GunRecoilCurveYaw;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* GunRecoilCurvePitch;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	FTimeline GunRecoilTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	float GunRecoilTimelineLength;

	FDoOnce RecoilPoint1;

	FDoOnce RecoilPoint2;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

public:
	UPROPERTY(EditAnywhere, Category = "BulletTrail")
	UNiagaraSystem* TrailParticle;

public:
	void WeaponDataInit(UPDA_WeaponData* _Data);
	void SocketDataInit(UPDA_GunSocketData* _Data);

public:
	virtual void OnInteract(AMyCharacter* Target) override;


public:
	UFUNCTION()
	void ChangeState(EGunState newGunState);

	UFUNCTION()
	void DropGun();

	TTuple<FVector, FRotator> GetMuzzlePosition();

	FName GetMuzzleSocketName();

	void DecreaseAmmo();

	void SetVisualMagazine(bool to_hide);

	float GetSoundRadius();

	int GetMaxClip();

	int GetCurrClip();

	class USoundBase* GetFireSound();

	void SpawnMagazine();

	void AddControllerPitchAndYaw(float _pitch, float _yaw);

	void UpdateAdsCameraPosition();

	class AADS_Camera* GetAdsCamera();

public:
	void SetVisualLaserSight(bool _On);

	void UpdateLaserSight(bool _Hit, FVector _Location);

public:
	FVector GetAdsCameraOffset();

	void UpdateCameraFOV();

	template<typename AttachmentType>
	AttachmentType* IsAttachmentExist(EAttachmentType _type)
	{
		if (IsValid(Attachments[(int)_type]))
		{
			return Cast<AttachmentType>(Attachments[(int)_type]);
		}

		return nullptr;
	}

	void AddAttachment(EAttachmentType _type);

	void RemoveAttachment(EAttachmentType _type);

public:
	void OnFire();

	void StopFire();

	void ResetRecoil();

	UFUNCTION()
	void GunRecoilUpdate();

	UFUNCTION()
	void GunRecoilFinished();

	virtual void Init_OnCreate();
};
