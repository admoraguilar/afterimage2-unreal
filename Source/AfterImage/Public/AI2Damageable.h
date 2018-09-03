// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAI2Damageable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class AFTERIMAGE_API IAI2Damageable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//classes using this interface must implement ReactToHighNoon
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Damageable")
		void TakeDamage(float AmmoData,float MinDamageModifier, float MaxDamageModifier, float CritDamageModifier, AActor* DamageCauser, FHitResult HitInformation);
};
