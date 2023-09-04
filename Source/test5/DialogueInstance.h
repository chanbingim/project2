// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Engine/DataTable.h"

#include "DialogueInstance.generated.h"

/**
 *
 */
UCLASS()
class TEST5_API UDialogueInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UDialogueInstance();
	virtual void Init() override;
	struct FDialogueData* GetDialogueData(int Row);
private:
	UPROPERTY()
	class UDataTable* DialogueTable;
};
