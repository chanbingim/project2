// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCustomEnums.h"
#include "MySequenceNodes.h"
#include "PDA_WeaponImpactDefinition.h"
#include "Components/TimeLineComponent.h"
#include "GunBase.h"
#include "WeaponToOwner.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST5_API UWeaponComponent : public UActorComponent, public IWeaponToOwner
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	TArray<class AGunBase*> GunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponSlot ChangeGunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponSlot CurrentGunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	class AGunBase* CurrentEquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UUIMainHUD* MainHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UDamageUI* DamageHUDWidget;
	UPROPERTY(VisibleAnywhere)
		int32 ClipNum;
	UPROPERTY()
		int32 ClipAllNum;
	UPROPERTY(VisibleAnywhere)
		int32 ClipNum2;
	UPROPERTY()
		int32 ClipAllNum2;
	UPROPERTY(VisibleAnywhere)
		int32 ClipNum3;
	UPROPERTY()
		int32 ClipAllNum3;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon1 = false;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon2 = false;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon3 = false;

	UPROPERTY()
		int32 RandomNumber;


	bool IsDamage = false;
	bool CanShoot;

	bool IsAiming;

	TArray<int> Ammo;
	//TArray<UTexture2D*> text;

	bool IsPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	bool IsReloading;

	bool IsShooting;
	
	UPDA_WeaponImpactDefinition* WeaponImpactEffect;

	float UserFov;

	bool ADSMode;

	bool KeepADSMode;

	void SetAimPointCamera(UCameraComponent* _NewCamera);

public:
	class UCameraComponent* AimPointCamera;

	float SpreadValue;

public:
	TSubclassOf<UCameraShakeBase> FiringShake;

	TSubclassOf<UCameraShakeBase> RecoilShake;

	UTexture2D* WeaponText1;
	UTexture2D* WeaponText2;
	UTexture2D* WeaponText3;

public:
	UFUNCTION()
	void PickUpGun(AGunBase* _NewGun);

	void UpdateCurrentGun();
	//void UpdateTexture();

	FName GetGunHoldSocketName(AGunBase* _Gun);

	FName GetGunHoldSocketName();

	void UpdateGunAttachment();

	TTuple<FVector, bool> GetLookat();
	
	void InitalizeWeaponComponent(class UCameraComponent* _AimCamera, TSubclassOf<AGunBase> _GunSlot1, TSubclassOf<AGunBase> _GunSlot2, TSubclassOf<AGunBase> _GunSlot3, bool _bPlayer = false);

	bool CheckEnoughAmmo();

	void ReloadGun();

	bool ReloadCheck();

	bool CheckCanShoot();

	void UpdateUI();

	TTuple<FVector, FRotator> GetFirePosition();

	void SpawnBulletTrail(FHitResult _HitData, FVector _TraceEnd, float _RayLength);

	void HitImpulse(FHitResult _HitData);

	void ImpactEffects(FHitResult _HitData);

	class UCameraComponent* GetADSCamera();

	FVector SpreadTrace(FVector _InputTrace);

public:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(EWeaponSlot _Slot);

	void WeaponShellEjection();

	void SpawnBullet();

	void PlayWeaponFireFx();

	void WeaponReload();

	void WeaponSelectorChange();

	void BurstFire();

	void GunFire();

	void FullAutoFire();

	void FullAutoFireRetrigger();

	void ReleaseFire();

	void WeaponSequence();

	void Texture();
	void SwapTexture();

	
public://�������̽�
	UFUNCTION(BlueprintCallable)
	virtual void ReloadAnimationComplete() override;

	UFUNCTION(BlueprintCallable)
	virtual void SlotChangeAnimationComplete() override;
};
