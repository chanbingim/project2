#include "ItemBase.h"
// Fill out your copyright notice in the Description page of Project Settings.

#include "../wqwe/MyCharacter.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "InteractionWidget.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(DefaultSceneComponent);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTWIDGET"));

	static ConstructorHelpers::FClassFinder<UUserWidget> UW(TEXT("/Game/WeaponSystem/Interaction/WBP_Interaction_2.WBP_Interaction_2_C"));

	if (UW.Succeeded())
	{
		WidgetComp->SetWidgetClass(UW.Class);
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawAtDesiredSize(true);
		WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WidgetComp->InitWidget();
		WidgetComp->SetupAttachment(SkeletalMesh);
	}

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	sphere->SetupAttachment(SkeletalMesh);
	sphere->SetSphereRadius(64.f);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	WidgetComp->SetVisibility(false);

	UInteractionWidget* WO = Cast<UInteractionWidget>(WidgetComp->GetUserWidgetObject());
	if (WO != nullptr)
	{
		WO->SetItemName(ItemName);
	}
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::OnInteract(AMyCharacter* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Interact"));
}

void AItemBase::OnInteractRangeEntered()
{
	UE_LOG(LogTemp, Log, TEXT("Entered"));

	WidgetComp->SetVisibility(true);
}

void AItemBase::OnInteractRangeExited()
{
	UE_LOG(LogTemp, Log, TEXT("Exited"));

	WidgetComp->SetVisibility(false);
}
