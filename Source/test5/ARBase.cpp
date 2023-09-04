// Fill out your copyright notice in the Description page of Project Settings.


#include "ARBase.h"

AARBase::AARBase()
{
	WeaponSlot = EWeaponSlot::WeaponSlot1;
}

void AARBase::SelectorChange()
{
	Selector = ESelectorType(((int)Selector + 1) % (int)ESelectorType::End);
}
