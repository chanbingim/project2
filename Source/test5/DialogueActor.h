// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InteractInterface.h"

#include "DialogueActor.generated.h"

UCLASS()
class TEST5_API ADialogueActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ADialogueActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//class UBoxComponent* InteractionBox;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UBoxComponent* Collider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		int DialogueId;
	bool IsUsed;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool IsInstInteraction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool IsInteractable;
	UPROPERTY(EditAnywhere, Category = "Dialogue")
		TArray<AActor*> DialogueActors;
	virtual void OnInteract(AMyCharacter* Target) override;
	virtual void OnInteractRangeEntered() override;
	virtual void OnInteractRangeExited() override;
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
