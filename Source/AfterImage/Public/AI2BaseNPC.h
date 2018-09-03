// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/Character.h"
#include "AI2Wanderpoint.h"
#include "AI2BaseNPC.generated.h"

UENUM(BlueprintType)
enum class EWorkTask : uint8 {
	WT_Idle,
	WT_Patrolling,
	WT_Wandering
};

UENUM(BlueprintType)
enum class EAggressivenessBehavior : uint8 {
	AB_InspectFirst,
	AB_ShootImmediately,
	AB_RunForHelp
};

UCLASS()
class AFTERIMAGE_API AAI2BaseNPC : public ACharacter
{
	GENERATED_BODY()
	bool bWasEventsBinded = false;
public:
	// Sets default values for this character's properties
	AAI2BaseNPC();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIGameplay")
		EWorkTask CurrentWorkTask;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIGameplay")
		EAggressivenessBehavior AggressionBehavior;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AIGameplay")
		AAI2Wanderpoint* WanderpointPath;
};
