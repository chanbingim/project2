// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "Styling/SlateBrush.h"
//#include "WeaponComponent.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"
#include "UIMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API UUIMainHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UProgressBar* HPBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UBorder* WeaponInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* WeaponSwapBackGround;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* BulletCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* Slash;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* BulletAllCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon1BackGround;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon2BackGround;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon3BackGround;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* WeaponName1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* WeaponName2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UTextBlock* WeaponName3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon11;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon22;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UImage* Weapon33;

	UPROPERTY()
		bool CheckWeapon1 = true;
	UPROPERTY()
		bool CheckWeapon2 = false;
	UPROPERTY()
		bool CheckWeapon3 = false;


	UPROPERTY()
		FTimerHandle timer;
	void SetHP(float fPercent);
	void SetBullet(FText bullet);
	void SetBulletAll(FText allbullet);

	void SetWeapon(UTexture2D* _Gun);

	void Weapon1PickVisi();


	void BackGroundVisi(UTexture2D* _GunText1, UTexture2D* _GunText2, UTexture2D* _GunText3);
	void BackGroundOffVisi();

protected:
	virtual void NativeConstruct() override;
};
