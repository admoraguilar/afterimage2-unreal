// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/Actor.h"
#include "AI2Interactable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopInteractEvent);
UCLASS()
class AFTERIMAGE_API AAI2Interactable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI2Interactable();

	UPROPERTY(BlueprintAssignable, Category = "Interact")
		FInteractEvent Interact;
	UPROPERTY(BlueprintAssignable, Category = "Interact")
		FStopInteractEvent StopInteract;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//UFUNCTION(BlueprintCallable, Category = "Interact")
	//	virtual void Interact(APawn* interactor) {}
};
