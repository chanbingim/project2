// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "ARBase.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API AARBase : public AGunBase
{
	GENERATED_BODY()

public:
	AARBase();
	
public:
	UPROPERTY()
	ESelectorType Selector;

	UPROPERTY()
	int BurstFireCount;

public:
	UFUNCTION()
	void SelectorChange();
};
