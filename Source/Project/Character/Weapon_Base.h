// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

UCLASS()
class PROJECT_API AWeapon_Base : public AActor {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Sphere;

	FTimerHandle Timer_Release;

public:
	// Sets default values for this actor's properties
	AWeapon_Base();

	UPROPERTY()
	class ACharacter_Base* Weapon_Owner = nullptr;

	bool bHasOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire", meta = (AllowPrivateAccess = "true"))
	int32 WeaponFireRate = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Fire", meta = (AllowPrivateAccess = "true"))
	int32 WeaponFireTimer = 0;

protected:
	bool bIsReload = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	bool bInfiniteAmmo = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Ammo_Max = 100;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Ammo_Current;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Clip_Ammo_Max = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Clip_Ammo_Current;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Get", meta = (AllowPrivateAccess = "true"))
	bool bGetAuto = false;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Get", meta = (AllowPrivateAccess = "true"))
	//bool bGetSound = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Get", meta = (AllowPrivateAccess = "true"))
	bool bGetPlayerOnly = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitWeapon();
	
	void GetWeapon(class ACharacter_Base* GetOwner);
	UFUNCTION(BlueprintCallable)
	void ReleaseWeapon();
	
	void Equipped();
	void UnEquipped();

	void BeginReload();
	void EndReload();

	virtual void Fire();

	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class USphereComponent* GetSphere() const { return Sphere; }
};
