// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "Character_PlayerController.generated.h"

UCLASS()
class PROJECT_API ACharacter_PlayerController : public APlayerController, public IGenericTeamAgentInterface {
	GENERATED_BODY()

public:
	ACharacter_PlayerController();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool team = false;
	
private: 
	FGenericTeamId TeamId = FGenericTeamId(5);	// Implement The Generic Team Interface
	virtual FGenericTeamId GetGenericTeamId() const override;

public:
	virtual void Tick(float DeltaTime) override;
};
