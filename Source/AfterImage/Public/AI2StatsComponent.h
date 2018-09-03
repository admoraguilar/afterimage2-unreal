// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "Components/ActorComponent.h"
#include "AI2StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AFTERIMAGE_API UAI2StatsComponent : public UActorComponent
{
	GENERATED_BODY()
	float CurrentHealth;
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
		float MAX_HEALTH=100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float REGEN_SPEED = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float REGEN_VALUE = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		float MOVEMENT_MULTIPLIER = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		bool IS_INVINCIBLE;
	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		bool IsDead = false;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FHealthEvent OnHealthDroppedZeroEvent;
	// Sets default values for this component's properties
	UAI2StatsComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	//returns is dead?
	UFUNCTION(BlueprintCallable, Category="Gameplay")
		bool TakeDamage(float Value);
	//returns is dead?
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		inline float GetHealth() { return CurrentHealth; }
	//set hp, mainly used for clone copying hp
	void SetHealth(float Health);
};
