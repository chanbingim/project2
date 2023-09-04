// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Materials/MaterialInterface.h"
#include "MyCustomEnums.h"

#include "PDA_WeaponImpactDefinition.generated.h"

/**
 * 
 */
UCLASS()
class TEST5_API UPDA_WeaponImpactDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> ImpactVFX;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> ImpactDecal;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, FVector> ImpactDecalSize;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, FVector2D> ImpactDecalScaleMultiplier;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, bool> ShouldImpactMakeNoise;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, float> ImpactNoiseRange;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, int> DecalMaxNumberOfVariations;

};
