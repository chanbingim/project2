// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachmentBase.h"
#include "LaserSightBase.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API ALaserSightBase : public AAttachmentBase
{
	GENERATED_BODY()

public:
	ALaserSightBase();
	
public:
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere)
	FName LaserStartSocket;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	bool LaserSightVisual;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	UParticleSystem* WeaponLaserSightFX;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	FLinearColor LaserSightColor;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	UParticleSystemComponent* LaserSightComponent;

public:
	UFUNCTION()
	FVector GetLaserStartSocket();

	UFUNCTION()
	FVector GetLaserForwardVector();

	TTuple<FVector, bool> GetLaserForwardTrace();

	UFUNCTION()
	void SpawnLaserEmitter();

	virtual void Init_OnCreate() override;
};
