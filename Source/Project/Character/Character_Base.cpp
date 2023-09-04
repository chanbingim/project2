// Fill out your copyright notice in the Description page of Project Settings.

#include "Character_Base.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Weapon_Base.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACharacter_Base::ACharacter_Base() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void ACharacter_Base::BeginPlay() {
	Super::BeginPlay();

	Weapon_Array.SetNum(1);
}

void ACharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacter_Base::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacter_Base::MoveRight);

	

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacter_Base::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacter_Base::TouchStopped);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharacter_Base::OnResetVR);


	
	PlayerInputComponent->BindAction("WeaponRelease", IE_Pressed, this, &ACharacter_Base::ReleaseWeapon);
	PlayerInputComponent->BindAction("WeaponReload", IE_Pressed, this, &ACharacter_Base::ReloadWeapon);
	PlayerInputComponent->BindAction("WeaponChange_Up", IE_Pressed, this, &ACharacter_Base::ChangeWeapon_Up);
	PlayerInputComponent->BindAction("WeaponChange_Down", IE_Pressed, this, &ACharacter_Base::ChangeWeapon_Down);
	PlayerInputComponent->BindAxis("WeaponFire", this, &ACharacter_Base::FireWeapon);
}

void ACharacter_Base::OnResetVR() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACharacter_Base::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location) {
	Jump();
}

void ACharacter_Base::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location) {
	StopJumping();
}

void ACharacter_Base::MoveForward(float Value) {
	if ((Controller != nullptr) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacter_Base::MoveRight(float Value) {
	if ( (Controller != nullptr) && (Value != 0.0f) ) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACharacter_Base::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	for (int32 Index = 1; Index < Weapon_Array.Num(); Index += 1) {
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan,
			FString::Printf(TEXT("%d : Weapon Name %s"),
				Index, *Weapon_Array[Index]->GetName()));
	
	}
	
	// GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan,
	// 		FString::Printf(TEXT("Weapon Array Num %d, Index %d"),
	// 			Weapon_Array.Num(), Weapon_Array_Index));
}

void ACharacter_Base::GetWeapon(AWeapon_Base* Weapon) {

	if (Weapon_Array.Num() < Weapon_Array_Max - 1) {
		Weapon_Array.Add(Weapon);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
			FString::Printf(TEXT("Weapon %s Get"), *Weapon->GetName()));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString::Printf(TEXT("Weapon Array Full")));
	}

	Weapon_Array_Index = Weapon_Array.Num() - 1;
	ChangeWeapon(Weapon_Array_Index);
}

void ACharacter_Base::ReleaseWeapon() {

	if (Weapon_Array_Index != 0)	{
		if (Weapon_Array.IsValidIndex(Weapon_Array_Index) == true) {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan,
				FString::Printf(TEXT("Weapon %s Release / Weapon Array Num %d"),
					*Weapon_Array[Weapon_Array_Index]->GetName(), Weapon_Array.Num()));
	
			Weapon_Array[Weapon_Array_Index]->ReleaseWeapon();
			Weapon_Array.RemoveAt(Weapon_Array_Index);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
				FString::Printf(TEXT("Release Index Error")));
		}
	}

	if (Weapon_Array_Index > Weapon_Array.Num() - 1) {
		Weapon_Array_Index = Weapon_Array.Num() - 1;
	}
	if (Weapon_Array.IsValidIndex(Weapon_Array_Index) == false)	{
		Weapon_Array_Index = 0;
	}
	ChangeWeapon(Weapon_Array_Index);

	
}

void ACharacter_Base::FireWeapon(float Value) {
	if (Value)	{
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red,
				FString::Printf(TEXT("Fire Input")));

		if (Weapon_Current) {
			Weapon_Current->Fire();
		}
	}
}

void ACharacter_Base::ReloadWeapon() {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
				FString::Printf(TEXT("Reload Input")));

	if (Weapon_Current) {
		Weapon_Current->BeginReload();
	}
}

void ACharacter_Base::ChangeWeapon(int32 Index) {
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
				FString::Printf(TEXT("Weapon Index %d"), Weapon_Array_Index));

	if (Index != 0)	{
		if (Weapon_Array.IsValidIndex(Index) == true) {
			Weapon_Current = Weapon_Array[Index];
		}
	}
	else {
		Weapon_Current = nullptr;
	}
}

void ACharacter_Base::ChangeWeapon_Up() {
	Weapon_Array_Index += 1;
	if (Weapon_Array_Index > Weapon_Array.Num() - 1) {
		Weapon_Array_Index = 0;
	}

	ChangeWeapon(Weapon_Array_Index);
}

void ACharacter_Base::ChangeWeapon_Down() {
	Weapon_Array_Index += -1;
	if (Weapon_Array_Index < 0) {
		Weapon_Array_Index = Weapon_Array.Num() - 1;
	}

	ChangeWeapon(Weapon_Array_Index);
}

bool ACharacter_Base::IsDead() {
	if (Health <= 0) {
		return true;
	}
	return false;
}
