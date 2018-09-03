// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2Character.h"
#include "AI2Interactable.h"
#include "GameFramework/Actor.h"
#include "AI2InteractionObserver.generated.h"

class AAI2Character;
UCLASS()
class AFTERIMAGE_API AAI2InteractionObserver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI2InteractionObserver();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	const float INTERACT_MINIMUM_DISTANCE = 200.0f;
	//TArray<AAI2Character*> InteractorObjects;
	TArray<AAI2Interactable*> InteractableObjects;
	AAI2Interactable* RequestForNearbyInteractable(AAI2Character* querier);

	UFUNCTION(BlueprintCallable, Category = "GameManagers")
		void GetInteractableReferences();
};
