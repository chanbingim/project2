// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGunState : uint8
{
	Dropped = 0 UMETA(DisplayName = "Dropped"),
	Equipped UMETA(DisplayName = "Equipped"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	WeaponNone = 0 UMETA(DisplayName = "WeaponNone"),
	WeaponSlot1 = 1 UMETA(DisplayName = "WeaponSlot1"),
	WeaponSlot2 = 2 UMETA(DisplayName = "WeaponSlot2"),
	WeaponSlot3 = 3 UMETA(DisplayName = "WeaponSlot3"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class ESelectorType : uint8
{
	Single = 0 UMETA(DisplayName = "Single"),
	Burst UMETA(DisplayName = "Burst"),
	FullAuto UMETA(DisplayName = "FullAuto"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol = 0 UMETA(DisplayName = "Pistol"),
	AssultRifle UMETA(DisplayName = "AssultRifle"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	Optics = 0 UMETA(DisplayName = "Optics"),
	Supressor UMETA(DisplayName = "Supressor"),
	LaserSight UMETA(DisplayName = "LaserSight"),
	End UMETA(DisplayName = "End"),
};

class TEST5_API MyCustomEnums
{
public:
	MyCustomEnums();
	~MyCustomEnums();
};
