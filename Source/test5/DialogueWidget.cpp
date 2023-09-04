// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "DialogueTable.h"
#include "Components/TextBlock.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDialogueWidget::UpdateDialogue(const FDialogueData* Dialogue)
{
	UE_LOG(LogTemp, Log, TEXT("Widget updated"));
	if (DialogueTextBlock != nullptr)
	{
		DialogueTextBlock->SetText(FText::FromString(Dialogue->text));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Textbox is nullptr"));
	}
}
