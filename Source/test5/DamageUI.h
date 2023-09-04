// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "DamageUI.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API UDamageUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Damage1Impact;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Damage2Impact;

	//bool : 재생//
	//AnimationName : 애니메이션 이름//
	//StartAtTime : 시작 시간//
	//NumLoopsToPlay : 루프 횟수//
	//PlayMode : 재생 모드//
	//PlaybackSpeed : 재생 속도//
	UPROPERTY(BlueprintReadOnly, Transient, Category = "GsUI", meta = (BindWidgetAnim))
		class UWidgetAnimation* DamageOpacity;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "GsUI", meta = (BindWidgetAnim))
		class UWidgetAnimation* DamageOpacity2;

	/*UPROPERTY()
		float color = 0.2f;*/
	UPROPERTY()
		float color = 0.0f;

	virtual void NativeConstruct() override;
	//virtual void NativeOnInitialized();

	void StartPlayAnimation();
	void StopPlayAnimation();
	void EventPlayAnimation();

	FLinearColor SetColor;
	FLinearColor EndColor;
	FTimerHandle timer;
	FWidgetAnimationDynamicEvent DamageOpacityEvent;

	void SetVisi();
	void EndVisi();
	void SetVisi2();
	void EndVisi2();
	void StartPlayAnimation2();
	void StopPlayAnimation2();

	void SetOp(float fOpacity);
};
