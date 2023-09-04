// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Base.h"

#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Character_Base.h"
#include "Character_Player.h"

// Sets default values
AWeapon_Base::AWeapon_Base() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetRelativeRotation(FRotator(270.0f, 0.0f, 0.0f));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("interactPreset"));
	// Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	// Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->InitSphereRadius(80.0f);
	Sphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void AWeapon_Base::BeginPlay() {
	Super::BeginPlay();

	Ammo_Current = Ammo_Max;
	Clip_Ammo_Current = Clip_Ammo_Max;

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon_Base::OnOverlapBegin);
}

void AWeapon_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (bHasOwner == false)	{
		if (bGetAuto == true) {
			ACharacter_Base* GetOwner = Cast<ACharacter_Base>(OtherActor);

			if (GetOwner != nullptr) {
				GetWeapon(GetOwner);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("AWeapon_Base::OnOverlapBegin Cast<ACharacter_Base> Fail")));
			}
		}
	}
}

// Called every frame
void AWeapon_Base::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	Ammo_Current = FMath::Min(Ammo_Current, Ammo_Max);
	Clip_Ammo_Current = FMath::Min(Clip_Ammo_Current, Clip_Ammo_Max);

	WeaponFireTimer += -1;
	WeaponFireTimer = FMath::Max(FMath::Min(WeaponFireTimer, WeaponFireRate), 0);
	if (Weapon_Owner) {
		if (Weapon_Owner->Weapon_Current == this) {
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green,
		FString::Printf(TEXT("Clip %d / Ammo %d"), Clip_Ammo_Current, Ammo_Current));
		}
	}
}

void AWeapon_Base::InitWeapon() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
			FString::Printf(TEXT("InitWeapon")));
}

void AWeapon_Base::GetWeapon(ACharacter_Base* GetOwner) {
	Weapon_Owner = GetOwner;
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
		FString::Printf(TEXT("WeaponOwner %s"), *Weapon_Owner->GetName()));

	bHasOwner = true;
	bGetAuto = false;
	
	Mesh->SetSimulatePhysics(false);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->AttachToComponent(Weapon_Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "ik_hand_r");

	Weapon_Owner->GetWeapon(this);
}

void AWeapon_Base::ReleaseWeapon() {
	if (bHasOwner == true) {
		Mesh->SetSimulatePhysics(true);
		//Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("%s"), *Weapon_Owner->GetActorForwardVector().ToString()));
		Mesh->AddImpulse(FVector(Weapon_Owner->GetActorForwardVector().X * 2000, Weapon_Owner->GetActorForwardVector().Y * 2000, 4000));

		GetWorld()->GetTimerManager().SetTimer(Timer_Release, this, &AWeapon_Base::InitWeapon, 1, false);
		
		Weapon_Owner = nullptr;
		bHasOwner = false;
	}
}

void AWeapon_Base::Equipped() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
	FString::Printf(TEXT("Weapon %s Equip"), *GetName()));
}

void AWeapon_Base::UnEquipped() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
	FString::Printf(TEXT("Weapon %s UnEquip"), *GetName()));
}

void AWeapon_Base::BeginReload() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
	FString::Printf(TEXT("Weapon %s Reload"), *GetName()));
	
	//탄약 존재시
	if (Ammo_Current > 0) {
		bIsReload = true;
		EndReload();
	}
	else {

	}
}

void AWeapon_Base::EndReload() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
	FString::Printf(TEXT("Weapon %s Reload Finish"), *GetName()));
	
	if (bInfiniteAmmo == false)	{
		const int32 Ammo_Need = (Clip_Ammo_Max - Clip_Ammo_Current) > Ammo_Current ? Ammo_Current : (Clip_Ammo_Max - Clip_Ammo_Current) ;
		Ammo_Current += -Ammo_Need;
		Clip_Ammo_Current += Ammo_Need;
	}
	else {
		Clip_Ammo_Current += Clip_Ammo_Max;
	}

	// 위젯 업데이트
	bIsReload = false;
}

void AWeapon_Base::Fire() {
	GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red,
	FString::Printf(TEXT("Weapon Fire Timer %d"), WeaponFireTimer));

	if ((Clip_Ammo_Current > 0) && bIsReload == false) {
		if (WeaponFireTimer <= 0) {
			Clip_Ammo_Current += -1;
			WeaponFireTimer = WeaponFireRate;
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red,
		FString::Printf(TEXT("Weapon %s No Clip Ammo"), *GetName()));
	}
}

