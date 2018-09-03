// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2StatsComponent.h"

// Sets default values for this component's properties
UAI2StatsComponent::UAI2StatsComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAI2StatsComponent::BeginPlay() {
	Super::BeginPlay();
	//init current health
	CurrentHealth = MAX_HEALTH;
}


// Called every frame
void UAI2StatsComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	/*
	//if not dead, do regenerate health
	if (IsDead && !IS_INVINCIBLE) {
		if (CurrentHealth < MAX_HEALTH) {
			FTimerDelegate RegenTimerDelegate;
			FTimerHandle RegenTimerHandle;
			RegenTimerDelegate.BindUFunction(this, FName("TakeDamage"), REGEN_VALUE);
			GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, RegenTimerDelegate, REGEN_SPEED, false);
		}
	}*/
}

bool UAI2StatsComponent::TakeDamage(float Value) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, (TEXT("TAKE DAMAGE!!!!")));
	CurrentHealth -= Value;
	if (CurrentHealth <= 0) {
		IsDead = true;
		OnHealthDroppedZeroEvent.Broadcast();
		return true;
	}
	return false;
}

void UAI2StatsComponent::SetHealth(float Health) {
	CurrentHealth = Health;
}