// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGameMode.h"
#include "wqwe/MyCharacter.h"
#include "WeaponComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DialogueWidget.h"
#include "DialogueTable.h"

AUIGameMode::AUIGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/WeaponSystem/Player/BP_Player2"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/WeaponSystem/Player/BP_Player_Controller.BP_Player_Controller_C"));
	if (PlayerControllerBPClass.Class != NULL) {
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// 블루프린트 클래스를 받아온다
	static ConstructorHelpers::FClassFinder<UUIMainHUD> MainHUDWidgetAsset(TEXT("/Game/UI/WBPUI"));
	static ConstructorHelpers::FClassFinder<UDamageUI> DamageHUDWidgetAsset(TEXT("/Game/UI/WBPDUI"));
	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairAsset(TEXT("/Game/UI/WBCrosshair"));
	static ConstructorHelpers::FClassFinder<UDialogueWidget>DialogueHUDAsset(TEXT("/Game/UI/WBP_Dialogue"));

	static ConstructorHelpers::FClassFinder<UUserWidget> SniperWidgetAsset(TEXT("/Game/WeaponSystem/CrossHair/UW_SniperScope.UW_SniperScope_C"));

	// TSubclassOf 템플릿 클래스 객체에 블루프린트 클래스를 넣어준다
	if (MainHUDWidgetAsset.Succeeded())
	{
		MainHUDWidgetClass = MainHUDWidgetAsset.Class;
	}
	if (DamageHUDWidgetAsset.Succeeded())
	{
		DamageHUDWidgetClass = DamageHUDWidgetAsset.Class;
	}
	if (CrossHairAsset.Succeeded())
	{
		CrossHairWidgetclass = CrossHairAsset.Class;
	}

	if (DialogueHUDAsset.Succeeded())
	{
		DialogueHUDClass = DialogueHUDAsset.Class;
	}

	if (SniperWidgetAsset.Succeeded())
	{
		SniperWidgetclass = SniperWidgetAsset.Class;
	}
	UE_LOG(LogTemp, Log, TEXT("%d"), DialogueHUDAsset.Class == nullptr ? 0 : 1);

	//if (false == DialogueHUDAsset.Class)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Invalid MAINHUD class"));

	//}
	DialogueLife = 0.f;
}

void AUIGameMode::UpdateDialogueWidget(const FDialogueData* Dialogue)
{
	DialogueHUDWidget->UpdateDialogue(Dialogue);
	DialogueHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	DialogueLife = 3.f;
}

void AUIGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DamageHUDWidgetClass))
	{
		DamageHUDWidget = Cast<UDamageUI>(CreateWidget(GetWorld(), DamageHUDWidgetClass));

		if (IsValid(DamageHUDWidget))
		{
			// 위젯을 뷰포트에 띄우는 함수
			DamageHUDWidget->AddToViewport();
		}
	}

	if (IsValid(MainHUDWidgetClass))
	{
		MainHUDWidget = Cast<UUIMainHUD>(CreateWidget(GetWorld(), MainHUDWidgetClass));

		if (IsValid(MainHUDWidget))
		{
			// 위젯을 뷰포트에 띄우는 함수
			MainHUDWidget->AddToViewport();
		}
	}

	if (IsValid(CrossHairWidgetclass))
	{
		CrossHairWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), CrossHairWidgetclass));

		if (IsValid(CrossHairWidget))
		{
			CrossHairWidget->AddToViewport();
			CrossHairWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (false == IsValid(MainHUDWidgetClass))
	{
		UE_LOG(LogTemp, Log, TEXT("Invalid HUD class"));
		return;
	}
	DialogueHUDWidget = Cast<UDialogueWidget>(CreateWidget(GetWorld(), DialogueHUDClass));
	if (false == IsValid(DialogueHUDWidget))
	{
		UE_LOG(LogTemp, Log, TEXT("Invalid HUD Widget"));
		return;
	}
	DialogueHUDWidget->AddToViewport();

	if (IsValid(SniperWidgetclass))
	{
		SniperWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), SniperWidgetclass));

		if (IsValid(SniperWidget))
		{
			SniperWidget->AddToViewport();
			SniperWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

}