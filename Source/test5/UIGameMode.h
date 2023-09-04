// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIMainHUD.h"
#include "DamageUI.h"
#include "GameFramework/GameModeBase.h"
#include "UIGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API AUIGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AUIGameMode();
	UUIMainHUD* GetMainHUDWidget() const
	{
		return MainHUDWidget;
	}
	UDamageUI* GetDamageHUDWidget() const
	{
		return DamageHUDWidget;
	}
	UUserWidget* GetCrossHairWidget() const
	{
		return CrossHairWidget;
	}
	UUserWidget* GetSniperWidget() const
	{
		return SniperWidget;
	}

	void UpdateDialogueWidget(const struct FDialogueData* Dialogue);
protected:
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<UUserWidget> MainHUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<UUserWidget> DamageHUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<UUserWidget> CrossHairWidgetclass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
		TSubclassOf<UUserWidget> SniperWidgetclass;

	UUIMainHUD* MainHUDWidget;
	UDamageUI* DamageHUDWidget;
	UUserWidget* CrossHairWidget;

	UUserWidget* SniperWidget;

#pragma region Dialogue by JY
	TSubclassOf<UUserWidget> DialogueHUDClass;
	class UDialogueWidget* DialogueHUDWidget;
	float DialogueLife;
#pragma endregion
};
