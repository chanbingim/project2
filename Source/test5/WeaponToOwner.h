// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MyCustomEnums.h"
#include "WeaponToOwner.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class UWeaponToOwner : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEST5_API IWeaponToOwner
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

		//Weapon to Owner
public:
	virtual void PlayReloadAnimation() {}
	virtual void PlayFireAnimation() {}
	virtual void PlaySlotHoldAnimation(EWeaponSlot _Slot) {}
	virtual void PlaySlotReleaseAnimation(EWeaponSlot _Slot) {}

		//Owner to Weapon
public:
	virtual void ReloadAnimationComplete() {}
	virtual void SlotChangeAnimationComplete() {}
};
