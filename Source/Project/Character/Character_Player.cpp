// Fill out your copyright notice in the Description page of Project Settings.

#include "Character_Player.h"

#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Weapon_Base.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACharacter_Player::ACharacter_Player() {
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void ACharacter_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacter_Player::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacter_Player::LookUpAtRate);
	
	PlayerInputComponent->BindAction("Get", IE_Pressed, this, &ACharacter_Player::Pick);
}

void ACharacter_Player::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacter_Player::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacter_Player::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan,
			FString::Printf(TEXT("Weapon Array Num %d, Index %d"),
				Weapon_Array.Num(), Weapon_Array_Index));

	if (Weapon_Array_Index > 0)	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan,
			FString::Printf(TEXT("Weapon Name %s"),
				*Weapon_Array[Weapon_Array_Index]->GetName()));
	
	}

	FrontCast();
}

FHitResult ACharacter_Player::FrontCast() {
	
	//FVector Start = GetActorLocation() + GetActorForwardVector() * f + FVector(0, 0, 40);
	//FVector End = GetActorLocation() + GetActorForwardVector() * f + FVector(0, 0, -120);

	FVector Start = GetCameraBoom()->GetComponentLocation() + FVector(0, 0, 40);
	FVector End = Start + GetFollowCamera()->GetForwardVector() * Pick_Distance;
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	bool bResult = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, ActorsToIgnore,
	EDrawDebugTrace::ForOneFrame, Hit, true, FLinearColor::Red, FLinearColor::Green, GetWorld()->GetDeltaSeconds());

	if (bResult == true) {
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Green,
		FString::Printf(TEXT("Hit %s"), *Hit.GetActor()->GetName()));
	}

	return Hit;
}

void ACharacter_Player::Pick() {
	FHitResult Hit = FrontCast();

	if (Hit.bBlockingHit == true) {

		AWeapon_Base* Weapon = Cast<AWeapon_Base>(Hit.GetActor());
		if (Weapon) {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("Hit %s"), *Weapon->GetName()));
			Weapon->GetWeapon(this);
		}
	}
}
