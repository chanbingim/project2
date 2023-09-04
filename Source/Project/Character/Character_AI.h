// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character_Base.h"
#include "CustomEnum.h"
#include "Components/TimeLineComponent.h"
#include "Character_AI.generated.h"

UCLASS()
class PROJECT_API ACharacter_AI : public ACharacter_Base {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* BillboardIcon;
	
public:
	ACharacter_AI();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Squad")
	class AAI_Squad* Squad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float Weapon_Range = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Patrol")
	E_EnemyPatrolType Patrol_Type = E_EnemyPatrolType::Patrol_Stay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Patrol|Spline")
	class AAI_Patrol_Path* Patrol_Spline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Patrol|Spline")
	int32 Patrol_Spline_Init = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Patrol|Spline")
	bool Patrol_Forward = true;
	
	FVector Patrol_Start_Location = FVector(0.0f, 0.0f, 0.0f);
	FRotator Patrol_Start_Rotator = FRotator(0.0f, 0.0f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Patrol|Random")
	float Patrol_Random_Range = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Search")
	E_EnemySearchType Search_Type = E_EnemySearchType::Search_Normal;

	FTimeline CurveTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Curve", Meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* Curve;

protected:
	FRotator Rotator_Start = FRotator(0.0f, 0.0f, 0.0f);
	FRotator Rotator_End = FRotator(0.0f, 0.0f, 0.0f);
	
	int32 Patrol_Spline_Index = 0;
	
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	FTransform GetNextPatrolPosition();
	
	void LookAtPoint(FVector Point);

	UFUNCTION()
	void TimelineProcess();
	
	void ShowIcon(E_EnemyState State);

	FORCEINLINE class UWidgetComponent* GetWidgetIcon() const { return WidgetIcon; }
	FORCEINLINE class UBillboardComponent* GetBillboardIcon() const { return BillboardIcon; }
};
