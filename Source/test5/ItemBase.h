// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "ItemBase.generated.h"

UCLASS()
class TEST5_API AItemBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneComponent;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* WidgetComp;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* sphere;

	UPROPERTY(VisibleAnywhere)
	FName ItemName;

public:
	virtual void OnInteract(class AMyCharacter* Target) override;
	virtual void OnInteractRangeEntered() override;
	virtual void OnInteractRangeExited() override;

public:
};
