// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ADS_Camera.h"
#include "SupressorBase.h"
#include "OpticsBase.h"
#include "LaserSightBase.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeaponComponent.h"

#include "ShellEjectionBase.h"


AGunBase::AGunBase()
{
	static ConstructorHelpers::FClassFinder<AADS_Camera> ADS(TEXT("/Script/CoreUObject.Class'/Script/test5.ADS_Camera'"));

	if (ADS.Succeeded())
	{
		ADSCameraSystem = CreateDefaultSubobject<UChildActorComponent>(TEXT("ADSCamera"));
		ADSCameraSystem->SetupAttachment(SkeletalMesh);
		ADSCameraSystem->SetChildActorClass(ADS.Class);
	}

	GunRecoilCurvePitch = nullptr;
	GunRecoilCurveYaw = nullptr;
	GunRecoilTimelineLength = 3.5f;

	MaxClip = 30;
	CurrClip = 0;

	Attachments.Init(nullptr, (int)EAttachmentType::End);

	RecoilMaxThresHold = 25.0f;
	RecoilRate = 5.0f;

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/ThirdPersonKit/Particles/P_BulletTracer.P_BulletTracer"));

	//if (Particle.Succeeded())
	//{
	//	TrailParticle = Particle.Object;
	//}
	
	ShellEjectionObj = AShellEjectionBase::StaticClass();

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve1(TEXT("/Game/WeaponSystem/Item/Guns/GunRecoil_PitchCurve.GunRecoil_PitchCurve"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/WeaponSystem/Item/Guns/GunRecoil_YawCurve.GunRecoil_YawCurve"));

	if (Curve1.Succeeded())
	{
		GunRecoilCurvePitch = Curve1.Object;
	}

	if (Curve2.Succeeded())
	{
		GunRecoilCurveYaw = Curve2.Object;
	}

	LaserClass = ALaserSightBase::StaticClass();
	OpticsClass = AOpticsBase::StaticClass();
	SupressorClass = ASupressorBase::StaticClass();
}

void AGunBase::BeginPlay()
{
	AItemBase::BeginPlay();

	if (GunRecoilCurvePitch != nullptr && GunRecoilCurveYaw != nullptr)
	{
		FOnTimelineFloat CurvePitchCallback;
		FOnTimelineFloat CurveYawCallback;
		FOnTimelineEvent TimelineFinishedCallback;

		CurvePitchCallback.BindUFunction(this, FName("GunRecoilUpdate"));
		CurveYawCallback.BindUFunction(this, FName("GunRecoilUpdate"));
		TimelineFinishedCallback.BindUFunction(this, FName("GunRecoilFinished"));

		GunRecoilTimeline.SetDirectionPropertyName(FName("TimelineDirection"));

		GunRecoilTimeline.AddInterpFloat(GunRecoilCurveYaw, CurvePitchCallback);
		GunRecoilTimeline.AddInterpFloat(GunRecoilCurvePitch, CurveYawCallback);
		GunRecoilTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		GunRecoilTimeline.SetTimelineLength(GunRecoilTimelineLength);
	}

	DropGun();

	WeaponDataInit(WeaponData);
	SocketDataInit(SocketData);
}

void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GunRecoilTimeline.TickTimeline(DeltaTime);
}

void AGunBase::WeaponDataInit(UPDA_WeaponData* _Data)
{
	if (IsValid(_Data))
	{
		ItemName = _Data->WeaponName;
		SkeletalMesh->SetSkeletalMesh(_Data->WeaponMesh);
		WeaponType = _Data->WeaponType;
		WeaponSlot = _Data->WeaponSlot;
		WeaponHUD = _Data->WeaponHUD;
		AmmoIndex = _Data->AmmoIndex;
		MaxClip = _Data->MaxClip;

		GunReloadSound = _Data->GunReloadSound;
		GunReloadAnim = _Data->GunReloadAnim;
		GunFireSound = _Data->GunFireSound;
		GunFireAnim = _Data->GunFireAnim;
		GunFireParticle = _Data->GunFireParticle;
		ShellEjectionObj = _Data->ShellEjectionObj;
		MagazineObj = _Data->MagazineObj;

		WeaponMinSpread = _Data->WeaponSpread.X;
		WeaponMaxSpread = _Data->WeaponSpread.Y;
		WeaponMinDamage = _Data->WeaponDamage.X;
		WeaponMaxDamage = _Data->WeaponDamage.Y;
		SoundRadius = _Data->SoundRadius;

		GunRecoilCurvePitch = _Data->GunRecoilCurvePitch;
		GunRecoilCurveYaw = _Data->GunRecoilCurveYaw;
		GunRecoilTimelineLength = _Data->GunRecoilTimelineLength;

		TrailParticle = _Data->TrailParticle;
	}
}

void AGunBase::SocketDataInit(UPDA_GunSocketData* _Data)
{
	if (IsValid(_Data))
	{
		MuzzleFlashName = _Data->MuzzleFlashName;
		SupressorMuzzleFlashName = _Data->SupressorMuzzleFlashName;
		AimingCameraLocation = _Data->AimingCameraLocation;
		AttachmentSocketLaser = _Data->AttachmentSocketLaser;
		AttachmentSocketSupressor = _Data->AttachmentSocketSupressor;
		AttachmentSocketScope = _Data->AttachmentSocketScope;
		BulletShellSpawnLocation = _Data->BulletShellSpawnLocation;
		HandGripTransform = _Data->HandGripTransform;
		Magazine = _Data->Magazine;
	}
}

void AGunBase::OnInteract(AMyCharacter* Target)
{
	Target->WeaponSystem->PickUpGun(this);
	SetOwner(Target);
}

void AGunBase::ChangeState(EGunState newGunState)
{
	GunState = newGunState;

	if (GunState == EGunState::Dropped)
	{
		sphere->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")), true);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetCollisionProfileName(FName(TEXT("DroppedGun")), true);
	}
	else if (GunState == EGunState::Equipped)
	{
		sphere->SetCollisionProfileName(FName(TEXT("NoCollision")), false);
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")), false);
		SkeletalMesh->ResetRelativeTransform();
	}
}

void AGunBase::DropGun()
{
	ChangeState(EGunState::Dropped);
	this->SetOwner(nullptr);
}

TTuple<FVector, FRotator> AGunBase::GetMuzzlePosition()
{
	FVector MuzzleLocation;
	FRotator MuzzleRotator;

	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		MuzzleLocation = SkeletalMesh->GetSocketLocation(SupressorMuzzleFlashName);
		MuzzleRotator = SkeletalMesh->GetSocketRotation(SupressorMuzzleFlashName);
	}
	else
	{
		MuzzleLocation = SkeletalMesh->GetSocketLocation(MuzzleFlashName);
		MuzzleRotator = SkeletalMesh->GetSocketRotation(MuzzleFlashName);
	}

	return MakeTuple(MuzzleLocation, MuzzleRotator);
}

FName AGunBase::GetMuzzleSocketName()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return SupressorMuzzleFlashName;
	}

	return MuzzleFlashName;
}

void AGunBase::DecreaseAmmo()
{
	CurrClip = FMath::Clamp(CurrClip - 1, 0, MaxClip);
}

void AGunBase::SetVisualMagazine(bool to_hide)
{
	if (to_hide)
	{
		SkeletalMesh->HideBoneByName(Magazine, EPhysBodyOp::PBO_None);
	}
	else
	{
		SkeletalMesh->UnHideBoneByName(Magazine);
	}
}

float AGunBase::GetSoundRadius()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return temp->SoundRadius;
	}

	return SoundRadius;
}

int AGunBase::GetMaxClip()
{
	return MaxClip;
}

int AGunBase::GetCurrClip()
{
	return CurrClip;
}

USoundBase* AGunBase::GetFireSound()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return temp->GetSupressorFireSound(WeaponType);
	}

	return GunFireSound;
}

void AGunBase::SpawnMagazine()
{
	if (MagazineObj)
	{
		MagazineVelocity = SkeletalMesh->GetSocketLocation(Magazine);
		
		float InDelayTime = GetWorld()->GetDeltaSeconds() * 5;

		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				MagazineVelocity = SkeletalMesh->GetSocketLocation(Magazine) - MagazineVelocity;

				FTransform MagazineTransform = SkeletalMesh->GetSocketTransform(Magazine);
				MagazineTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();

				AActor* SpawnedMagazine = GetWorld()->SpawnActor<AActor>(MagazineObj, MagazineTransform.GetLocation(), MagazineTransform.Rotator(), SpawnParams);

				SkeletalMesh->IgnoreActorWhenMoving(SpawnedMagazine, false);
				
				SetVisualMagazine(false);

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}
		), InDelayTime, false);
	}
}

void AGunBase::AddControllerPitchAndYaw(float _pitch, float _yaw)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	PlayerController->AddPitchInput(_pitch);
	PlayerController->AddYawInput(_yaw);
}

void AGunBase::UpdateAdsCameraPosition()
{
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());
	ADS_Camera->Camera->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, AimingCameraLocation);

	AOpticsBase* Optic = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);

	if (IsValid(Optic))
	{
		ADS_Camera->Camera->SetRelativeLocation(Optic->CameraOffset);
	}
}

AADS_Camera* AGunBase::GetAdsCamera()
{
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	return ADS_Camera;
}

void AGunBase::SetVisualLaserSight(bool _On)
{
	ALaserSightBase* Laser = IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);

	if (IsValid(Laser))
	{
		Laser->LaserSightVisual = _On;
	}
}

void AGunBase::UpdateLaserSight(bool _hit, FVector _Location)
{
	ALaserSightBase* Laser = IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	if (IsValid(Laser))
	{
		if (IsValid(Laser->LaserSightComponent))
		{
			FVector LaserStartLocation = Laser->GetLaserStartSocket();
			FVector TraceEndLocation = _Location;

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			CollisionParams.AddIgnoredActor(Laser);

			GetWorld()->LineTraceSingleByChannel(HitResult, LaserStartLocation, TraceEndLocation, ECollisionChannel::ECC_Visibility, CollisionParams);

			FVector LaserEndLocation = HitResult.bBlockingHit ? HitResult.Location : _Location;

			Laser->LaserSightComponent->SetBeamSourcePoint(0, LaserStartLocation, 0);
			Laser->LaserSightComponent->SetBeamTargetPoint(0, LaserEndLocation, 0);
		}
	}
}

FVector AGunBase::GetAdsCameraOffset()
{
	FVector Offset = { 0, 0, 0 };

	AOpticsBase* Optics = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);

	if (IsValid(Optics))
	{
		Offset = Optics->CameraOffset;
	}

	return Offset;
}

void AGunBase::UpdateCameraFOV()
{
	AOpticsBase* Optics = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	float Fov = DefaultCameraFOV;

	if (IsValid(Optics))
	{
		Fov = Optics->CameraFOV;
	}

	ADS_Camera->SettingCameraFOV(Fov);
}

void AGunBase::AddAttachment(EAttachmentType _type)
{
	if (IsValid(IsAttachmentExist<AAttachmentBase>(_type)))
	{
		RemoveAttachment(_type);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	TSubclassOf<AAttachmentBase> SpawnClass;
	FName AttachSocket;

	switch (_type)
	{
	case EAttachmentType::Optics:
		SpawnClass = OpticsClass;
		AttachSocket = AttachmentSocketScope;
		break;
	case EAttachmentType::Supressor:
		SpawnClass = SupressorClass;
		AttachSocket = AttachmentSocketSupressor;
		break;
	case EAttachmentType::LaserSight:
		SpawnClass = LaserClass;
		AttachSocket = AttachmentSocketLaser;
		break;
	}


	if (SpawnClass != nullptr)
	{
		AAttachmentBase* NewAttachment = GetWorld()->SpawnActor<AAttachmentBase>(SpawnClass, SpawnParams);
		NewAttachment->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
		Attachments[(int)_type] = NewAttachment;

		NewAttachment->Init_OnCreate();
	}
}

void AGunBase::RemoveAttachment(EAttachmentType _type)
{
	AAttachmentBase* RemoveAttachment = IsAttachmentExist<AAttachmentBase>(_type);
	
	if (IsValid(RemoveAttachment))
	{
		RemoveAttachment->Destroy();
	}
}

void AGunBase::OnFire()
{
	GunRecoilTimeline.SetPlayRate(1.f);
	TimelineDirection = ETimelineDirection::Forward;

	if (RecoilPoint1.Execute())
	{
		OriginRotation=GetWorld()->GetFirstPlayerController()->GetControlRotation();
	}

	GunRecoilTimeline.Play();
}

void AGunBase::StopFire()
{
	GunRecoilTimeline.SetPlayRate(RecoilRate);
	TimelineDirection = ETimelineDirection::Backward;
	GunRecoilTimeline.Reverse();
}

void AGunBase::ResetRecoil()
{
	RecoilPoint1.Reset();
	RecoilPoint2.Reset();
}

void AGunBase::GunRecoilUpdate()
{
	float TimelineValue = GunRecoilTimeline.GetPlaybackPosition();

	float PitchAmount = GunRecoilCurvePitch->GetFloatValue(TimelineValue);
	float YawAmount = GunRecoilCurveYaw->GetFloatValue(TimelineValue);



	if (TimelineDirection.GetValue() == ETimelineDirection::Forward)
	{
		AddControllerPitchAndYaw(PitchAmount, YawAmount);
	}
	else if (TimelineDirection.GetValue() == ETimelineDirection::Backward)
	{
		if (RecoilPoint2.Execute())
		{
			PostRecoilRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
		}

		FRotator Rot = UKismetMathLibrary::NormalizedDeltaRotator(OriginRotation, PostRecoilRotation);

		float ClampValue = UKismetMathLibrary::MapRangeClamped(abs(Rot.Pitch), 0, RecoilMaxThresHold, 0, 1);

		PitchAmount = PitchAmount * -1 * ClampValue * RecoilRate;
		YawAmount = YawAmount * -1 * ClampValue * RecoilRate;

		AddControllerPitchAndYaw(PitchAmount, YawAmount);
	}
}

void AGunBase::GunRecoilFinished()
{
	if (TimelineDirection.GetValue() == ETimelineDirection::Backward)
	{
		ResetRecoil();
	}
}

void AGunBase::Init_OnCreate()
{
	int size = static_cast<int>(EAttachmentType::End);

	TArray<bool> OnSpawn_Attachments;
	OnSpawn_Attachments.SetNum(size);

	OnSpawn_Attachments[static_cast<int>(EAttachmentType::Optics)] = OnSpawn_Optic;
	OnSpawn_Attachments[static_cast<int>(EAttachmentType::LaserSight)] = OnSpawn_Laser;
	OnSpawn_Attachments[static_cast<int>(EAttachmentType::Supressor)] = OnSpawn_Supressor;

	for (int i = 0; i < size; i++)
	{
		bool flag = OnSpawn_Attachments[i];

		if (flag)
		{
			AddAttachment(EAttachmentType(i));
		}
	}
}
