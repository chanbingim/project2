// Fill out your copyright notice in the Description page of Project Settings.

#include "Test.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ATest::ATest() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void ATest::BeginPlay() {
	
}

// Called every frame
void ATest::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
