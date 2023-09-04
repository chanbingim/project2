// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoverSystemPublicData.h"
#include "AICoverPoint.generated.h"

UCLASS()
class WQWE_API AAICoverPoint : public AActor {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> Root;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ACoverSystem> Cover_System;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	TArray<FCover> Cover_List;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover", meta = (AllowPrivateAccess = "true"))
	TMap<FCoverHandle, TObjectPtr<class AMyCharacterAI_Controller>> Cover_Usable;

public:
	AAICoverPoint();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void RefreshCover(FVector Location);
	
	bool PickCover(FCover &Cover, TObjectPtr<class AMyCharacterAI_Controller> Controller, float Length);
};
