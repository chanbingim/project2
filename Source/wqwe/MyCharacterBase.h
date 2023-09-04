// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacterBase.generated.h"

UCLASS()
class WQWE_API AMyCharacterBase : public ACharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterBase();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;

	UPROPERTY(BlueprintReadOnly)
	bool bJog = false;

	UPROPERTY(BlueprintReadOnly)
	bool bAim = false;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterState")
	float HP_Max = 100.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterState")
	float HP = HP_Max;

	bool bDeathFound = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool GetDead();

	UFUNCTION(BlueprintCallable)
	virtual void GetDamage(float Damage);

	virtual void ActiveDead();

	UFUNCTION(BlueprintCallable)
	bool IsDead();
};
