// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "WeaponToOwner.h"
#include "ADS_Camera.h"
#include "ShellEjectionBase.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "LaserSightBase.h"

#include "SRBase.h"
#include "ARBase.h"
#include "MyCharacterAI.h"
#include "PistolBase.h"
#include "Perception/AISense_Damage.h"
#include "test5/UIGameMode.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GunSlot.SetNum((int)EWeaponSlot::End);
	Ammo.SetNum((int)EWeaponSlot::End);
	//text.SetNum((int)EWeaponSlot::End);
	Ammo[0] = 500;
	Ammo[1] = 510;
	Ammo[2] = 520;

	static ConstructorHelpers::FObjectFinder<UDataAsset> DataAsset(TEXT("/Game/WeaponSystem/Table/DA_WeaponImpactData.DA_WeaponImpactData"));

	if (DataAsset.Succeeded())
	{
		WeaponImpactEffect = Cast<UPDA_WeaponImpactDefinition>(DataAsset.Object);
	}

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeData1(TEXT("/Game/WeaponSystem/Component/BP_FiringShake.BP_FiringShake_C"));
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeData2(TEXT("/Game/WeaponSystem/Component/BP_RecoilShake.BP_RecoilShake_C"));

	if (ShakeData1.Succeeded())
	{
		FiringShake = ShakeData1.Class;
	}

	if (ShakeData2.Succeeded())
	{
		RecoilShake = ShakeData2.Class;
	}
	// ...

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj(TEXT("/Game/UI/Sprite/Xenon_Star/Sliced/FPS_Aspect/Weapon_Slot/weapon_example.weapon_example"));

	if (TextureObj.Succeeded())
	{
		WeaponText1 = Cast<UTexture2D>(TextureObj.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj2(TEXT("/Game/UI/Sprite/Xenon_Star/Sliced/FPS_Aspect/Weapon_Slot/weapon_example.weapon_example"));

	if (TextureObj2.Succeeded())
	{
		WeaponText2 = Cast<UTexture2D>(TextureObj2.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj3(TEXT("/Game/UI/Sprite/Xenon_Star/Sliced/FPS_Aspect/Weapon_Slot/weapon_example.weapon_example"));

	if (TextureObj3.Succeeded())
	{
		WeaponText3 = Cast<UTexture2D>(TextureObj3.Object);
	}








	CurrentGunSlot = EWeaponSlot::WeaponNone;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AUIGameMode* GameMode = Cast<AUIGameMode>(GetWorld()->GetAuthGameMode());
	MainHUDWidget = GameMode->GetMainHUDWidget();
	DamageHUDWidget = GameMode->GetDamageHUDWidget();
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	srand(time(NULL));
	RandomNumber = rand() % 2;
	if (IsValid(CurrentEquipWeapon))
	{
		ALaserSightBase* Laser = CurrentEquipWeapon->IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);
		TTuple<FVector, bool> Value;

		if (IsValid(Laser))
		{
			if (IsAiming)
			{
				Value = GetLookat();
			}
			else
			{
				Value = Laser->GetLaserForwardTrace();
			}

			FVector _Loc = Value.Get<0>();
			bool _hit = Value.Get<1>();

			CurrentEquipWeapon->UpdateLaserSight(_hit, _Loc);
		}
	}

	FString ClipText;
	FString ClipAllText;
	//FString ClipText2;
	//FString ClipAllText2;
	//FString ClipText3;
	//FString ClipAllText3;

	if (SwapWeapon1)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
	}
	else if (SwapWeapon2)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum2);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum2);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
	}
	else if (SwapWeapon3)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum3);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum3);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
	}
}
	

void UWeaponComponent::SetAimPointCamera(UCameraComponent* _NewCamera)
{
	AimPointCamera = _NewCamera;
}

void UWeaponComponent::PickUpGun(AGunBase* _NewGun)
{
	AGunBase* CurrentSlotGun = GunSlot[(int)_NewGun->WeaponSlot];

	FString ClipAllText;
	FString ClipAllText2;
	FString ClipAllText3;

	if (IsValid(CurrentSlotGun))
	{
		CurrentSlotGun->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentSlotGun->DropGun();

		GunSlot[(int)_NewGun->WeaponSlot] = nullptr;
	}

	GunSlot[(int)_NewGun->WeaponSlot] = _NewGun;
	_NewGun->ChangeState(EGunState::Equipped);

	UpdateCurrentGun();
	UpdateGunAttachment();


	if (_NewGun->AmmoIndex == 0)
	{
		ClipAllNum = Ammo[_NewGun->AmmoIndex];
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum);
	}
	else if (_NewGun->AmmoIndex == 1)
	{
		ClipAllNum2 = Ammo[_NewGun->AmmoIndex];
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum2);
	}
	else if (_NewGun->AmmoIndex == 2)
	{
		ClipAllNum3 = Ammo[_NewGun->AmmoIndex];
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum3);
	}
}

void UWeaponComponent::UpdateCurrentGun()
{
	CurrentEquipWeapon = GunSlot[(int)CurrentGunSlot];
}


FName UWeaponComponent::GetGunHoldSocketName(AGunBase* _Gun)
{
	FName SocketName = "None";

	if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot1)
	{
		SocketName = "GunStowedSocket1";
	}
	else if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot2)
	{
		SocketName = "GunStowedSocket2";
	}
	else if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot3)
	{
		SocketName = "GunStowedSocket3";
	}

	return SocketName;
}

FName UWeaponComponent::GetGunHoldSocketName()
{
	FName SocketName = "None";

	if (IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
		{
			SocketName = "PistolHoldSocket";
		}
		else
		{
			SocketName = "RifleHoldSocket";
		}
	}
	

	return SocketName;
}

void UWeaponComponent::UpdateGunAttachment()
{
	ACharacter* OwnChar = Cast<ACharacter>(GetOwner());

	if (IsValid(OwnChar))
	{
		for (AGunBase* Cur : GunSlot)
		{
			if (IsValid(Cur))
			{
				Cur->AttachToComponent(OwnChar->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetGunHoldSocketName(Cur));
			}
		}

		if (IsValid(GunSlot[(int)CurrentGunSlot]) && CurrentGunSlot!=EWeaponSlot::WeaponNone)
		{
			GunSlot[(int)CurrentGunSlot]->AttachToComponent(OwnChar->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetGunHoldSocketName());
		}
	}
}

TTuple<FVector, bool> UWeaponComponent::GetLookat()
{
	FVector Start = AimPointCamera->GetComponentLocation();
	FVector End = (AimPointCamera->GetComponentLocation() + (AimPointCamera->GetForwardVector() * 10000.f));

	FVector ResultVector;
	bool ResultHit;

	FHitResult HitData;

	FCollisionQueryParams Params;

	if(GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECollisionChannel::ECC_Visibility))
	{
		ResultVector = HitData.Location;
		ResultHit = true;
	}
	else
	{
		ResultVector = HitData.TraceEnd;
		ResultHit = false;
	}
	

	return MakeTuple(ResultVector, ResultHit);
}

void UWeaponComponent::InitalizeWeaponComponent(UCameraComponent* _AimCamera, TSubclassOf<AGunBase> _GunSlot1, TSubclassOf<AGunBase> _GunSlot2, TSubclassOf<AGunBase> _GunSlot3, bool _bPlayer)
{
	AimPointCamera = _AimCamera;

	FTransform Location;
	Location.SetLocation(FVector(0.f, 0.f, 0.f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	IsPlayer = _bPlayer;

	TArray<TSubclassOf<AGunBase>> CreateArray;

	CreateArray.Add(_GunSlot1);
	CreateArray.Add(_GunSlot2);
	CreateArray.Add(_GunSlot3);

	for (TSubclassOf<AGunBase> GunClass : CreateArray)
	{
		if (GunClass)
		{
			AGunBase* CreateGun = GetWorld()->SpawnActor<AGunBase>(GunClass, Location, SpawnParams);
			CreateGun->Init_OnCreate();
			PickUpGun(CreateGun);
		}
	}

}

bool UWeaponComponent::CheckEnoughAmmo()
{
	if (IsValid(CurrentEquipWeapon))
	{
		return (CurrentEquipWeapon->CurrClip > 0);
	}

	return false;
}

void UWeaponComponent::ReloadGun()
{
	if (!IsReloading)
	{
		IsReloading = true;

		if (IsValid(CurrentEquipWeapon))
		{
			int NeedAmmo = CurrentEquipWeapon->GetMaxClip() - CurrentEquipWeapon->GetCurrClip();

			if (Ammo[CurrentEquipWeapon->AmmoIndex] >= NeedAmmo)
			{
				Ammo[CurrentEquipWeapon->AmmoIndex] -= NeedAmmo;
				CurrentEquipWeapon->CurrClip = CurrentEquipWeapon->MaxClip;
			}
			else
			{
				CurrentEquipWeapon->CurrClip += Ammo[CurrentEquipWeapon->AmmoIndex];

				Ammo[CurrentEquipWeapon->AmmoIndex] = 0;
			}

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Reload Complete"));

			//?�중??지?�것!
			if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
			{
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
			{
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::SniperRifle)
			{
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}

			//나중에 지울것!

			CheckCanShoot();

			IWeaponToOwner* Interface = Cast<IWeaponToOwner>(GetOwner());

			if (Interface)
			{
				Interface->PlayReloadAnimation();
			}
		}
	}
}

bool UWeaponComponent::ReloadCheck()
{
	if (IsValid(CurrentEquipWeapon))
	{
		//R
		return !(Ammo[CurrentEquipWeapon->AmmoIndex] <= 0 || CurrentEquipWeapon->MaxClip == CurrentEquipWeapon->CurrClip);
	}

	return false;
}

bool UWeaponComponent::CheckCanShoot()
{
	//R
	CanShoot = CheckEnoughAmmo() && !IsReloading;

	return CanShoot;
}

void UWeaponComponent::UpdateUI()
{
}

TTuple<FVector, FRotator> UWeaponComponent::GetFirePosition()
{
	FVector FirePosition;
	FRotator FireRotator;

	if (IsValid(CurrentEquipWeapon))
	{
		TTuple<FVector, FRotator> MuzzlePosition = CurrentEquipWeapon->GetMuzzlePosition();
		
		FirePosition = MuzzlePosition.Get<0>() + (UKismetMathLibrary::GetForwardVector(MuzzlePosition.Get<1>()) * 5.f);
		FireRotator = MuzzlePosition.Get<1>();
	}

	return MakeTuple(FirePosition, FireRotator);
}

void UWeaponComponent::SpawnBulletTrail(FHitResult _HitData, FVector _TraceEnd, float _RayLength)
{
	UNiagaraSystem* Particle = CurrentEquipWeapon->TrailParticle;

	FVector StartLocation = GetFirePosition().Get<0>();
	FVector VectorParam = _HitData.bBlockingHit ? _HitData.Location : _TraceEnd;

	UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(CurrentEquipWeapon->SkeletalMesh, Particle, StartLocation,
		UKismetMathLibrary::FindLookAtRotation(StartLocation, _TraceEnd), FVector::OneVector, true, true, ENCPoolMethod::AutoRelease, true);
	
	float Range = (_HitData.Distance / _RayLength + 1.0f) + (_HitData.bBlockingHit * -1);

	if (IsValid(Effect))
	{
		Effect->SetFloatParameter(FName("TracerLifeTime"), Range);
		Effect->SetVectorParameter(FName("EndPoint"), _TraceEnd);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("SpawnBulletTrail::Can't Create Particle"));
	}

	

	//Effect->SetFloatParameter(FName("TracerLifetime"), Range);
	//Effect->SetVectorParameter(FName("ShockBeamEnd"), VectorParam);
	// 
	//UParticleSystemComponent* CreateParticle = UGameplayStatics::SpawnEmitterAtLocation
	//(GetWorld(), Particle, StartLocation, UKismetMathLibrary::FindLookAtRotation(StartLocation, _TraceEnd), false);

	//float Range = (_HitData.Distance / _RayLength + 1.0f) + (_HitData.bBlockingHit * -1);
	//
	//CreateParticle->SetFloatParameter(FName("TracerLifetime"), Range);
	//CreateParticle->SetVectorParameter(FName("ShockBeamEnd"), VectorParam);
}

void UWeaponComponent::HitImpulse(FHitResult _HitData)
{
	//UStaticMeshComponent* HitObject = Cast<UStaticMeshComponent>(_HitData.Component);

	//if (IsValid(HitObject))
	//{
	//	if (HitObject->IsSimulatingPhysics())
	//	{
	//		if (HitObject->Mobility == EComponentMobility::Movable)
	//		{
	//			FVector Impulse;
	//			HitObject->AddImpulseAtLocation(Impulse, _HitData.Location);
	//		}
	//	}
	//}
}

void UWeaponComponent::ImpactEffects(FHitResult _HitData)
{
	if(IsValid(WeaponImpactEffect))
	{
		EPhysicalSurface Surface = UGameplayStatics::GetSurfaceType(_HitData);

		//FX
		{
			UParticleSystem* Particle = *(WeaponImpactEffect->ImpactVFX.Find(Surface));

			if ((WeaponImpactEffect->ImpactVFX.Find(Surface)!=nullptr))
			{
				FVector temp = _HitData.ImpactNormal;
				float Rand_f = UKismetMathLibrary::RandomFloatInRange(-180.f, 180.f);

				FRotator Rotation = UKismetMathLibrary::MakeRotator(temp.X + Rand_f, temp.Y, temp.Z);

				FVector Scale = UKismetMathLibrary::MakeVector(1.f, 1.f, 1.f) * UKismetMathLibrary::RandomFloatInRange(0.9f, 1.1f);

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, UKismetMathLibrary::MakeTransform(_HitData.Location, Rotation, Scale));
			}
		}
		//Decal
		{
			UMaterialInterface* ImpactDecal = *(WeaponImpactEffect->ImpactDecal.Find(Surface));
			FVector2D ImpactDecalScaleMultiplier = *(WeaponImpactEffect->ImpactDecalScaleMultiplier.Find(Surface));
			FVector ImpactDecalSize = *(WeaponImpactEffect->ImpactDecalSize.Find(Surface));
			int DecalMaxNumberOfVariation = *(WeaponImpactEffect->DecalMaxNumberOfVariations.Find(Surface));

			if ((WeaponImpactEffect->ImpactDecal.Find(Surface))!=nullptr)
			{
				FVector temp = _HitData.ImpactNormal;
				float Rand_f = UKismetMathLibrary::RandomFloatInRange(-180.f, 180.f);

				FRotator Rotation = UKismetMathLibrary::MakeRotator(temp.X + Rand_f, temp.Y + 180.f, temp.Z);

				FVector Size = ImpactDecalSize * UKismetMathLibrary::RandomFloatInRange(ImpactDecalScaleMultiplier.X, ImpactDecalScaleMultiplier.Y);

				float Life = 10.0f;

				UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecal, Size, _HitData.ImpactPoint, Rotation, Life);

				if (IsValid(Decal))
				{
					//찾�? 못한경우 1
					int maxValue = (WeaponImpactEffect->DecalMaxNumberOfVariations.Find(Surface) == nullptr) ? 1 : DecalMaxNumberOfVariation;

					Decal->CreateDynamicMaterialInstance()->SetScalarParameterValue(FName("Frame"), UKismetMathLibrary::RandomIntegerInRange(1, maxValue));
				}
			}
		}
		//Sound
		{
			USoundCue* ImpactSound = *(WeaponImpactEffect->ImpactSound.Find(Surface));

			if ((WeaponImpactEffect->ImpactSound.Find(Surface)) != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, _HitData.Location);
			}
		}
	}
}

UCameraComponent* UWeaponComponent::GetADSCamera()
{
	AADS_Camera* AdsCamera = Cast<AADS_Camera>(CurrentEquipWeapon->ADSCameraSystem->GetChildActor());

	if (AdsCamera)
	{
		return AdsCamera->Camera;
	}

	return nullptr;
}

FVector UWeaponComponent::SpreadTrace(FVector _InputTrace)
{
	float ClampedSpread = UKismetMathLibrary::MapRangeClamped(SpreadValue, 0.0f, 1.0f, 0.2f, 0.04f);
	float RandMax = CurrentEquipWeapon->WeaponMaxSpread * ClampedSpread;
	float RandMin = CurrentEquipWeapon->WeaponMinSpread * ClampedSpread;

	FVector OutputTrace;
	OutputTrace.X = _InputTrace.X + UKismetMathLibrary::RandomFloatInRange(RandMin, RandMax);
	OutputTrace.Y = _InputTrace.Y + UKismetMathLibrary::RandomFloatInRange(RandMin, RandMax);
	OutputTrace.Z = _InputTrace.Z + UKismetMathLibrary::RandomFloatInRange(RandMin, RandMax);

	return OutputTrace;
}

void UWeaponComponent::EquipWeapon(EWeaponSlot _Slot)
{
	//?�재 총을 ?�고?�는 ?�롯�?같�??�을경우�??�행
	if (CurrentGunSlot != _Slot)
	{

		//if (IsValid(GunSlot[(int)_Slot]))
		//{
			CanShoot = true;
			CurrentGunSlot = _Slot;
			UpdateCurrentGun();
			CurrentEquipWeapon = GunSlot[(int)CurrentGunSlot];
			//UpdateGunAttachment();
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Changed Slot"));
			if (IsValid(GunSlot[(int)_Slot]))
			{
				Texture();
			}

			if (CurrentGunSlot == EWeaponSlot::WeaponSlot1)
			{
				SwapWeapon1 = true;
				SwapWeapon2 = false;
				SwapWeapon3 = false;
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
			}

			else if (CurrentGunSlot == EWeaponSlot::WeaponSlot2)
			{
				SwapWeapon1 = false;
				SwapWeapon2 = true;
				SwapWeapon3 = false;
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}

			else if (CurrentGunSlot == EWeaponSlot::WeaponSlot3)
			{
				SwapWeapon1 = false;
				SwapWeapon2 = false;
				SwapWeapon3 = true;
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
	/*	}*/

		UpdateGunAttachment();

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Changed Slot"));
	}
}

void UWeaponComponent::WeaponShellEjection()
{
	if (CurrentEquipWeapon->ShellEjectionObj)
	{
		FTransform ShellSpawnTransform = CurrentEquipWeapon->SkeletalMesh->GetSocketTransform(FName("BulletShellSpawnLocation"));
		ShellSpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AShellEjectionBase* SpawnedShell = GetWorld()->SpawnActor<AShellEjectionBase>(CurrentEquipWeapon->ShellEjectionObj, ShellSpawnTransform.GetLocation(), ShellSpawnTransform.Rotator(), SpawnParams);

		SpawnedShell->Mesh->SetSimulatePhysics(true);
		SpawnedShell->Mesh->SetCollisionProfileName(FName("DroppedGun"));
		SpawnedShell->SetLifeSpan(10.0f);

		FVector Impulse = 
			(SpawnedShell->Mesh->GetForwardVector() * UKismetMathLibrary::RandomFloatInRange(1.f, 5.f))
			+ (SpawnedShell->Mesh->GetRightVector() * UKismetMathLibrary::RandomFloatInRange(-20.f, -50.f));

		SpawnedShell->Mesh->AddImpulse(Impulse);
	}
}

void UWeaponComponent::SpawnBullet()
{
	if (IsValid(CurrentEquipWeapon))
	{
		float RayLength = 10000.0f;
		PlayWeaponFireFx();
		WeaponSequence();
		
		FHitResult PreResult;
		FVector PreStart = AimPointCamera->GetComponentLocation();
		FVector PreEnd = PreStart + AimPointCamera->GetForwardVector() * RayLength;
		
		if (IsPlayer == false) {
			float tmp = 10.0f * RayLength / 100;
			float x = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			float y = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			float z = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			PreEnd += FVector(x, y, z);
		}
		GetWorld()->LineTraceSingleByChannel(PreResult, PreStart, PreEnd, ECollisionChannel::ECC_GameTraceChannel2);
		//DrawDebugLine(GetWorld(), PreStart, PreEnd, FColor::Green, false, 10, 0 , 3);
		if (IsPlayer == false) {
			PreResult.Location += FVector(0, 0, -100.0f);
		}
		
		FHitResult HitResult;
		FVector TraceStart = GetFirePosition().Get<0>();
		FVector TraceEnd = (PreResult.bBlockingHit == false) ? AimPointCamera->GetComponentLocation() + (SpreadTrace(AimPointCamera->GetForwardVector()) * RayLength)
			: PreResult.Location + (SpreadTrace(AimPointCamera->GetForwardVector()) * (PreResult.Distance / RayLength));
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10, 0 , 2);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel2))
		{
			ImpactEffects(HitResult);
			HitImpulse(HitResult);
			
			float WeaponBaseDamage = UKismetMathLibrary::RandomFloatInRange(CurrentEquipWeapon->WeaponMinDamage, CurrentEquipWeapon->WeaponMaxDamage);

			if (IsPlayer == true) {
				AMyCharacterAI* Character = Cast<AMyCharacterAI>(HitResult.GetActor());
				if (Character) {
					if (HitResult.BoneName == FName("head")) {
						Character->GetDamage(Character->HP_Max);
					}
					else {
						Character->GetDamage(WeaponBaseDamage);
						UAISense_Damage::ReportDamageEvent(GetWorld(), Character, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), 0.0f, HitResult.TraceStart, HitResult.Location);
					}
					//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s %f"), *Character->GetName(), Character->HP));
				}
			}
			else {
				AMyCharacter* Player = Cast<AMyCharacter>(HitResult.GetActor());
				if (Player) {
					IsDamage = true;
					Player->GetDamage(WeaponBaseDamage);
					if (IsValid(MainHUDWidget))
					{
						MainHUDWidget->SetHP(Player->HP / Player->HP_Max);
					}
					if (IsValid(DamageHUDWidget))
					{
						if (IsDamage)
						{
							if (RandomNumber == 0)
								DamageHUDWidget->SetVisi();
							else if (RandomNumber == 1)
								DamageHUDWidget->SetVisi2();
						}
					}
					IsDamage = false;
					//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s %f"), *Character->GetName(), Character->HP));
				}
				
				
			}
			//UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponBaseDamage, GetOwner()->GetInstigatorController(), CurrentEquipWeapon, nullptr);
		}
		
		SpawnBulletTrail(HitResult, TraceEnd, RayLength);
	}
}

void UWeaponComponent::PlayWeaponFireFx()
{
	if (IsValid(CurrentEquipWeapon))
	{

		CurrentEquipWeapon->SkeletalMesh->PlayAnimation(CurrentEquipWeapon->GunFireAnim, false);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentEquipWeapon->GetFireSound(), CurrentEquipWeapon->GetMuzzlePosition().Get<0>());


		UNiagaraFunctionLibrary::SpawnSystemAttached(CurrentEquipWeapon->GunFireParticle, CurrentEquipWeapon->SkeletalMesh, CurrentEquipWeapon->GetMuzzleSocketName()
			, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
	}
}

void UWeaponComponent::WeaponReload()
{
	if (ReloadCheck())
	{
		//IsReloading = true;
		ReloadGun();
		FVector SoundLocation = GetOwner()->GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentEquipWeapon->GunReloadSound, SoundLocation);
		CurrentEquipWeapon->SkeletalMesh->PlayAnimation(CurrentEquipWeapon->GunReloadAnim, false);

		IWeaponToOwner* Interface = Cast<IWeaponToOwner>(GetOwner());
		if (Interface)
		{
			Interface->PlayReloadAnimation();
		}

		if (true == ADSMode)
		{
			KeepADSMode = true;

			AMyCharacter* Player = Cast<AMyCharacter>(GetOwner());

			if (IsValid(Player))
			{
				//Disable FPP Mode
				Player->FPP_Mode();
			}
		}

		//�׽�Ʈ��
		IsReloading = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Can't Reload"));
	}
}

void UWeaponComponent::WeaponSelectorChange()
{
	if (!IsShooting && IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
		{
			AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

			Rifle->SelectorChange();

			ReleaseFire();
		}
	}
}

void UWeaponComponent::BurstFire()
{
	static int n = 0;

	if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
	{
		AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

		if (CanShoot && n < 3)
		{
			if (Rifle->Selector == ESelectorType::Burst)
			{
				float InDelayTime = 0.1f;

				IsShooting = true;

				SpawnBullet();
				if (IsPlayer) {
					CurrentEquipWeapon->OnFire();
				}
				n++;

				FTimerHandle TH;
				GetWorld()->GetTimerManager().SetTimer(TH, FTimerDelegate::CreateLambda([&]()
					{
						BurstFire();

						GetWorld()->GetTimerManager().ClearTimer(TH);
					}
				), InDelayTime, false);
			}
		}
		else
		{
			CurrentEquipWeapon->StopFire();
			n = 0;
		}
	}
}

void UWeaponComponent::GunFire()
{
	if (IsValid(CurrentEquipWeapon) && !IsReloading)
	{
		if (CheckEnoughAmmo())
		{
			switch (CurrentEquipWeapon->WeaponType)
			{
				case EWeaponType::Pistol:
				{
					SpawnBullet();
				}
				break;
				case EWeaponType::AssultRifle:
				{
					CheckCanShoot();

					AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

					if (Rifle->Selector == ESelectorType::Burst) {
						BurstFire();
					}
					else if (Rifle->Selector == ESelectorType::FullAuto) {
						FullAutoFire();
					}
					else {
						SpawnBullet();
					}

				}
				break;
				case EWeaponType::SniperRifle:
				{
					SpawnBullet();
				}
				break;
			}
		}
		else
		{
			if (ReloadCheck()) 
			{
				//ReloadGun();

				WeaponReload();
			}
		}
	}
}

void UWeaponComponent::FullAutoFire()
{
	if (IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
		{
			AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

			if (Rifle->Selector == ESelectorType::FullAuto)
			{
				IsShooting = true;
				FullAutoFireRetrigger();
			}
		}
	}
}

void UWeaponComponent::FullAutoFireRetrigger()
{
	if (IsShooting && CanShoot)
	{
		float InDelayTime = 0.1f;

		SpawnBullet();
		CurrentEquipWeapon->OnFire();

		FTimerHandle TH;
		GetWorld()->GetTimerManager().SetTimer(TH, FTimerDelegate::CreateLambda([&]()
			{
				FullAutoFireRetrigger();

				GetWorld()->GetTimerManager().ClearTimer(TH);
			}
		), InDelayTime, false);
	}
	else
	{
		IsShooting = false;

		CurrentEquipWeapon->StopFire();
	}
}

void UWeaponComponent::ReleaseFire()
{
	IsShooting = false;
}

void UWeaponComponent::WeaponSequence()
{
	if (IsValid(CurrentEquipWeapon))
	{
		CurrentEquipWeapon->DecreaseAmmo();
		CheckCanShoot();
		UpdateUI();
		WeaponShellEjection();

		if (IsPlayer)
		{
			if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
			{
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
			{
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::SniperRifle)
			{
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}

			if (IsValid(FiringShake))
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(FiringShake);
			}

			if (IsValid(RecoilShake))
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(RecoilShake);
			}
		}
	}
}

void UWeaponComponent::ReloadAnimationComplete()
{
	IsReloading = false;

	if (true == IsPlayer)
	{
		if (true == KeepADSMode)
		{
			KeepADSMode = false;
			AMyCharacter* Player = Cast<AMyCharacter>(GetOwner());
			
			if (IsValid(Player))
			{
				//Resume FPP Mode
				Player->FPP_Mode();
			}
		}
	}
}

void UWeaponComponent::SlotChangeAnimationComplete()
{
}

void UWeaponComponent::Texture()
{
	MainHUDWidget->SetWeapon(CurrentEquipWeapon->WeaponHUD);
}

void UWeaponComponent::SwapTexture()
{
	MainHUDWidget->BackGroundVisi(WeaponText1,WeaponText2,WeaponText3);
}
