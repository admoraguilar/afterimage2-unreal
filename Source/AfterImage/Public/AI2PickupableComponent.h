// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "Components/ActorComponent.h"
#include "AI2Character.h"
#include "AI2PickupableComponent.generated.h"

class AAI2Character;
// ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) 
UCLASS()
class AFTERIMAGE_API UAI2PickupableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAI2PickupableComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/*
	virtual void ActorBeginOverlap(AActor* OtherActor);

	void Pickup(AAI2Character* character);
	*/
};
