// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueInstance.h"
#include "DialogueTable.h"

UDialogueInstance::UDialogueInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable>DataTable(TEXT("/Game/Event/Dialogue/InteractionTable.InteractionTable"));
	if (false == DataTable.Succeeded()) return;
	DialogueTable = DataTable.Object;
}

void UDialogueInstance::Init()
{
	Super::Init();
}

FDialogueData* UDialogueInstance::GetDialogueData(int Row)
{
	return DialogueTable->FindRow<FDialogueData>(*FString::FromInt(Row), TEXT(""));
}