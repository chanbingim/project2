// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMainHUD.h"

void UUIMainHUD::NativeConstruct()
{
	Super::NativeConstruct();

	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	WeaponInfo = Cast<UBorder>(GetWidgetFromName(TEXT("WeaponInfo")));
	WeaponSwapBackGround = Cast<UImage>(GetWidgetFromName(TEXT("WeaponSwapBackGround")));
	Weapon1BackGround = Cast<UImage>(GetWidgetFromName(TEXT("Weapon1BackGround")));
	Weapon2BackGround = Cast<UImage>(GetWidgetFromName(TEXT("Weapon2BackGround")));
	Weapon3BackGround = Cast<UImage>(GetWidgetFromName(TEXT("Weapon3BackGround")));
	WeaponName1 = Cast<UTextBlock>(GetWidgetFromName(TEXT("WeaponName1")));
	WeaponName2 = Cast<UTextBlock>(GetWidgetFromName(TEXT("WeaponName2")));
	WeaponName3 = Cast<UTextBlock>(GetWidgetFromName(TEXT("WeaponName3")));
	BulletCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("BulletCount")));
	Slash = Cast<UTextBlock>(GetWidgetFromName(TEXT("Slash")));
	BulletAllCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("BulletAllCount")));
	Weapon1 = Cast<UImage>(GetWidgetFromName(TEXT("Weapon1")));
	Weapon11 = Cast<UImage>(GetWidgetFromName(TEXT("Weapon11")));
	Weapon22 = Cast<UImage>(GetWidgetFromName(TEXT("Weapon22")));
	Weapon33 = Cast<UImage>(GetWidgetFromName(TEXT("Weapon33")));
}

void UUIMainHUD::SetHP(float fPercent)
{
	if (IsValid(HPBar))
		HPBar->SetPercent(fPercent);
}

void UUIMainHUD::SetBullet(FText bullet)
{
	if (IsValid(BulletCount))
		BulletCount->SetText(bullet);
}

void UUIMainHUD::SetBulletAll(FText allbullet)
{
	if (IsValid(BulletAllCount))
		BulletAllCount->SetText(allbullet);
}

void UUIMainHUD::SetWeapon(UTexture2D* _Gun)
{
	Weapon1->SetBrushFromTexture(_Gun, true);
	Weapon1PickVisi();

	CheckWeapon1 = true;
	CheckWeapon2 = false;
	CheckWeapon3 = false;
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	WeaponSwapBackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon1BackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon2BackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon3BackGround->SetVisibility(ESlateVisibility::Hidden);
	WeaponName1->SetVisibility(ESlateVisibility::Hidden);
	WeaponName2->SetVisibility(ESlateVisibility::Hidden);
	WeaponName3->SetVisibility(ESlateVisibility::Hidden);
	BulletCount->SetVisibility(ESlateVisibility::Visible);
	BulletAllCount->SetVisibility(ESlateVisibility::Visible);
}

void UUIMainHUD::Weapon1PickVisi()
{
	Weapon1->SetVisibility(ESlateVisibility::Visible);
	Weapon11->SetVisibility(ESlateVisibility::Hidden);
	Weapon22->SetVisibility(ESlateVisibility::Hidden);
	Weapon33->SetVisibility(ESlateVisibility::Hidden);
}

void UUIMainHUD::BackGroundVisi(UTexture2D* _GunText1, UTexture2D* _GunText2, UTexture2D* _GunText3)
{
	Weapon11->SetBrushFromTexture(_GunText1, true);
	Weapon22->SetBrushFromTexture(_GunText2, true);
	Weapon33->SetBrushFromTexture(_GunText3, true);
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.2f);
	WeaponSwapBackGround->SetVisibility(ESlateVisibility::Visible);
	Weapon1BackGround->SetVisibility(ESlateVisibility::Visible);
	Weapon2BackGround->SetVisibility(ESlateVisibility::Visible);
	Weapon3BackGround->SetVisibility(ESlateVisibility::Visible);
	WeaponName1->SetVisibility(ESlateVisibility::Visible);
	WeaponName2->SetVisibility(ESlateVisibility::Visible);
	WeaponName3->SetVisibility(ESlateVisibility::Visible);
	Weapon11->SetVisibility(ESlateVisibility::Visible);
	Weapon22->SetVisibility(ESlateVisibility::Visible);
	Weapon33->SetVisibility(ESlateVisibility::Visible);
}

void UUIMainHUD::BackGroundOffVisi()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
	WeaponSwapBackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon1BackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon2BackGround->SetVisibility(ESlateVisibility::Hidden);
	Weapon3BackGround->SetVisibility(ESlateVisibility::Hidden);
	WeaponName1->SetVisibility(ESlateVisibility::Hidden);
	WeaponName2->SetVisibility(ESlateVisibility::Hidden);
	WeaponName3->SetVisibility(ESlateVisibility::Hidden);
	Weapon11->SetVisibility(ESlateVisibility::Hidden);
	Weapon22->SetVisibility(ESlateVisibility::Hidden);
	Weapon33->SetVisibility(ESlateVisibility::Hidden);
}