// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachmentBase.h"
#include "OpticsBase.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API AOpticsBase : public AAttachmentBase
{
	GENERATED_BODY()
public:
	AOpticsBase();

public:
	UPROPERTY(EditAnywhere)
	FVector ObjectOffset;

	UPROPERTY(EditAnywhere)
	FVector CameraOffset;

	UPROPERTY(EditAnywhere)
	float CameraFOV;

public:
	virtual void Init_OnCreate() override;
};
