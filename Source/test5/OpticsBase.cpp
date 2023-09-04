// Fill out your copyright notice in the Description page of Project Settings.


#include "OpticsBase.h"

AOpticsBase::AOpticsBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/WeaponSystem/FPS_Weapon_Bundle/Weapons/Meshes/Accessories/SM_T4_Sight.SM_T4_Sight"));

	if (SM.Succeeded())
	{
		AttachmentMesh->SetStaticMesh(SM.Object);
	}
}

void AOpticsBase::Init_OnCreate()
{
	Super::Init_OnCreate();

	SetActorRelativeLocation(ObjectOffset);
}