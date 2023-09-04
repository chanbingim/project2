// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShellEjectionBase.generated.h"

UCLASS()
class TEST5_API AShellEjectionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShellEjectionBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	class USceneComponent* DefaultSceneComponent;

	class USphereComponent* Sphere;

	class UStaticMeshComponent* Mesh;
};
