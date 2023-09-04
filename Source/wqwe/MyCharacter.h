// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define Max_Walk 300

#include "CoreMinimal.h"
#include "MyCharacterBase.h"
#include "MyCharacterAI_Controller.h"
#include "InputActionValue.h"
#include "Components/TimeLineComponent.h"
#include "../test5/WeaponToOwner.h"

#include "MyCharacter.generated.h"

UCLASS()
class WQWE_API AMyCharacter : public AMyCharacterBase, public IWeaponToOwner
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleCollider;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* AI_POS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* LeftCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Colision, meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* ourParitcleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Head;
public:
	// Sets default values for this character's properties
	AMyCharacter();

	

	// 우원씨가 추가한거
	/*void HeadDamageSwitch();
	void BodyDamageSwitch();
	void ArmLegDamageSwitch();
	void MinDamageSwitch();
	void ResetHPSwitch();*/
	//void ShootSwitch();
	//void ReloadSwitch();
	void StartShoot();
	void StopShoot();
	void StopDamage();

	void Weapon1SwapSwitch();
	void Weapon2SwapSwitch();
	void Weapon3SwapSwitch();

	//void BackGroundOnSwitch();
	void BackGroundOffSwitch();
	void BackGround();

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimeLineCurve;

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimeLineAimingCurve;

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimeCrouchingCapsuleCurve;

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimeCrouchingCameraCurve;

private:
	FTimeline timeLine;
	FOnTimelineFloat TimeLineUpdatedelegate;

	FTimeline timeLineAiming;
	FOnTimelineFloat TimeLineAimingUpdatedelegate;

	FTimeline SmoothCrouchingCurveTimeLine;
	FOnTimelineFloat TimeLineCrouchCapsuleUpdatedelegate;
	FOnTimelineFloat TimeLineCrouchCameraUpdatedelegate;

	UPROPERTY()
	class AMyCharacterAI* AiPawn;
	UPROPERTY()
	class AMyCharacterAI_Controller* OtherAI;

	UFUNCTION()
	void TimeLineUpdateFunc(float Output);

	UFUNCTION()
	void TimeLineAimingUpdateFunc(float Output);

	UFUNCTION()
	void TimeLineCrouchUpdateFunc(float Capsule);

	UFUNCTION()
	void TimeLineCrouchCameraUpdateFunc(float Camera);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Is_Spirnt = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Is_crouch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Is_Zoom = false;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	float Forward;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	bool ChangeCameraPos;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	bool MoveDirection;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	float Left;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	bool pos_WallHide;

	UPROPERTY(VisibleAnywhere, Category = PawnState)
	bool IsHang = false;

	UPROPERTY(VisibleAnywhere, Category = Wall)
	FVector Wall_Loc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	FVector Wall_Normal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	float PlayerHeight;
	UPROPERTY(VisibleAnywhere, Category = Wall)
	FVector Wall_Height;

	UPROPERTY()
	class UCharacte_Ani* Anim;

	UPROPERTY()
	class UCharacte_Ani* Anim2;

	UFUNCTION()
	void OffSet_AIm();

	UFUNCTION()
	void HangMove(float Value);

	UFUNCTION()
	void MoveDelay_Character();

	UFUNCTION()
	void StartValut();

	UFUNCTION()
	void CameraWibbingFun();

	float Max_Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnRoll", meta = (AllowPrivateAccess = "true"))
	bool Is_Roll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PawnValut", meta = (AllowPrivateAccess = "true"))
	bool Is_Valut;

	UPROPERTY(VisibleAnywhere)
	float opval = 0.0f;

	FTimerHandle timer;
	UPROPERTY(VisibleAnywhere)
	bool isFiring;

	UPROPERTY(VisibleAnywhere)
	bool isDamage;

	UPROPERTY()
	float Time = 0.0f;

	UPROPERTY(VisibleAnywhere)
	bool ChooseWeapon1 = true;
	UPROPERTY(VisibleAnywhere)
	bool ChooseWeapon2 = false;
	UPROPERTY(VisibleAnywhere)
	bool ChooseWeapon3 = false;

	UPROPERTY()
	bool isSwap;

	UPROPERTY()
	int32 RandomNum;

	UFUNCTION()
	void Aiming();

	UFUNCTION()
	void AimingEnd();
public:
	void OnInteraction();

	void Fire();

	void FireRelease();

	void ChangeSelector();

	void ChangeSlot1();

	void ChangeSlot2();

	void ChangeSlot3();

	void ReloadStart();

	/*virtual void PlayReloadAnimation() override;
	virtual void PlayFireAnimation() override;
	virtual void PlaySlotHoldAnimation(EWeaponSlot _Slot) override;
	virtual void PlaySlotReleaseAnimation(EWeaponSlot _Slot) override;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<TSubclassOf<class AGunBase>> GunSlot;

	// 우원씨가 추가한거
	// 내가 추가한거
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HeadDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BodyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ArmLegDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ResetHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Shoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Reload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UUIMainHUD* MainHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UDamageUI* DamageHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UUserWidget* CrossHairWidget;

	class UUserWidget* SniperWidget;

	bool Wall_HeightResult;
	bool Wall_NormalResult;
	bool Wall_HeadTrace;
	bool ValutTraceResult;
	bool ValutTraceHeightResult;
	bool ValutTraceThickResult;
	bool CheckClimbResult;
	bool CheckRollDistanceResult;

	bool CheckCornerRightResult;
	bool CheckCornerRight2Result;
	bool CheckUpWallResult;
	bool CheckHangResult;

	bool CheckCornerLeftResult;
	bool CheckCornerLeft2Result;
	bool CheckAmsalResult;

	bool flag = false;
	bool CornerAni = false;
	bool HangUp = false;

	FVector Direction2;

	FVector Actor_Loc;
	FRotator Player_Way;

	FVector Camera_Loc;
	FVector CameraFor_Loc;

	int32 HideTime = 0;
	
	int32 DeathTime = 0;
	bool Play_Amsal = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	bool PlayerDeath = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UWeaponComponent* WeaponSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponSlot TempEWeapon;
	virtual void PlayReloadAnimation() override;	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (DisplayName = "PlayReloadAnimation", ScriptName = "PlayReloadAnimation"))
	void K2_PlayReloadAnimation();
	virtual void K2_PlayReloadAnimation_Implementation() {};

	virtual void PlayFireAnimation() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (DisplayName = "PlayFireAnimation", ScriptName = "PlayFireAnimation"))
	void K2_PlayFireAnimation();
	virtual void K2_PlayFireAnimation_Implementation() {};

	virtual void PlaySlotHoldAnimation(EWeaponSlot _Slot) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (DisplayName = "PlaySlotHoldAnimation", ScriptName = "PlaySlotHoldAnimation"))
	void K2_PlaySlotHoldAnimation(EWeaponSlot _Slot);
	virtual void K2_PlaySlotHoldAnimation_Implementation(EWeaponSlot _Slot) {};

	virtual void PlaySlotReleaseAnimation(EWeaponSlot _Slot) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (DisplayName = "PlaySlotReleaseAnimation", ScriptName = "PlaySlotReleaseAnimation"))
	void K2_PlaySlotReleaseAnimation(EWeaponSlot _Slot);
	virtual void K2_PlaySlotReleaseAnimation_Implementation(EWeaponSlot _Slot) {};

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void FPP_Mode();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	void TurnLook(float VALUE);
	void LookUp(float VALUE);

	void MoveForward(float VALUE);
	void LeftRight(float VALUE);

	void Wall_HideMoveForWard(float VALUE);
	void Wall_HideLeftRight(float Value, FRotator Rot);

	UFUNCTION()
	void GetTraceValut();

	UFUNCTION()
	void GetTraceValutHeight();

	UFUNCTION()
	void GetTraceValutThick();

	UFUNCTION()
	void CheckHangWall();

	UFUNCTION()
	void EndMontaged(UAnimMontage* Montage, bool bInterrupted);

	//앉기 기능구현
	void StartCrouch();

	//구르기 기능 구현
	void Start_Roll();

	void Wall_Hide(float Value);

	//Sprint
	void Do_Sprint();

	void End_Sprint();

	void GetNormal();

	void GetPlayerUp();

	void CheckAmsal();

	void StartAmsal();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	FRotator Camera_Way;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterState)
	bool Hang_Slove = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterState)
	bool Low_Hide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterState)
	float Crouch_Walkspeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterState)
	float Sprint_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterState)
	float Zoom_Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnState)
	float Aim_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnState)
	float Aim_Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float Search_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float Search_HeightDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float Search_LeftDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float RollDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnState)
	float Search_RightEndDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	FVector ValutLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	FVector ValutNormal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	FVector Player_Up;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	bool Walls_Tall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	bool Walls_Thick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	float ValutorMantle = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnValut)
	FVector ValutHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	int Get_Weapon = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAmsal;

	UFUNCTION()
	void Get_Trace();

	UFUNCTION()
	void Check_Climb();

	UFUNCTION()
	void Over_Wall();

	UFUNCTION()
	void Move_Roll();

	UFUNCTION()
	void Hang_Jump();

	UFUNCTION()
	void MoveEnd_Roll();

	UFUNCTION()
	void ClimbEndNotify();

	UFUNCTION(BlueprintImplementableEvent)
	void Amsal_BP();

	UFUNCTION()
	void EndHang();

	UFUNCTION()
	void EndHide();

	UFUNCTION()
	void TraceRollDistance();

	UFUNCTION()
	void CapsuleRight();

	UFUNCTION()
	void CapsuleLeft();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Move_WallLeft = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Move_WallRight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PawnState", meta = (AllowPrivateAccess = "true"))
	bool Roll_Move = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnTime", meta = (AllowPrivateAccess = "true"))
	int32 CheckHideTime = 0;

	inline bool Get_crocuhState() { return Is_crouch; }
	inline bool Get_SprintState() { return Is_Spirnt; }
	inline bool Get_WallHide() { return pos_WallHide; }
	inline bool Get_LowWallHide() { return Low_Hide; }
	inline int Get_Weapons() { return Get_Weapon; }

	inline float Get_Forward() { return Forward; }
	inline float Get_Left() { return Left; }
	inline float Get_Yaw() { return Aim_Yaw; }
	inline float Get_Pitch() { return Aim_Pitch; }
	inline bool Get_Hang() { return IsHang; }
	inline bool Get_Direction() { return MoveDirection; }
};