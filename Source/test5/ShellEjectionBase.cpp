// Fill out your copyright notice in the Description page of Project Settings.


#include "ShellEjectionBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AShellEjectionBase::AShellEjectionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	


	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(DefaultSceneComponent);
	Sphere->SetCollisionProfileName("NoCollision");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/WeaponSystem/FPS_Weapon_Bundle/Weapons/Meshes/Ammunition/SM_Shell_556x45_Empty.SM_Shell_556x45_Empty"));

	if (SM.Succeeded())
	{
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		Mesh->SetupAttachment(Sphere);
		Mesh->SetStaticMesh(SM.Object);

		FTransform Trans; 
		FRotator Rot(-90.f, 75.f, 0.f);
		FVector Scale(2.f, 2.f, 2.f);

		Trans.SetRotation(FQuat(Rot));
		Trans.SetScale3D(Scale);

		Mesh->SetRelativeTransform(Trans);
	}
}

// Called when the game starts or when spawned
void AShellEjectionBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShellEjectionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

