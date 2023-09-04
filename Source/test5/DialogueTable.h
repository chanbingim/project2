// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

#include "Engine/Datatable.h"

#include "GameFramework/Actor.h"
#include "DialogueTable.generated.h"

UENUM(BlueprintType)
enum class DlgAppCon : uint8
{
	CON_OVERLAPPING,
	CON_INTERACTION,
	CON_CONTINUOUS
};

USTRUCT(BlueprintType)
struct FDialogueData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		FName text_Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		int text_Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		int text_Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		int sound_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		FString text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		DlgAppCon con_Appear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		int con_After;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		bool con_Skip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		float time_delete;
};

UCLASS()
class TEST5_API ADialogueTable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADialogueTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};