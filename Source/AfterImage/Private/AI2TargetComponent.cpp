// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2TargetComponent.h"


// Sets default values for this component's properties
UAI2TargetComponent::UAI2TargetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAI2TargetComponent::BeginPlay()
{
	Super::BeginPlay();



	// ...
	
}


// Called every frame
void UAI2TargetComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UAI2TargetComponent::AddBodyPart(FString partName, FTransform partTransform) {
	FBodyPartData newTargettable = FBodyPartData(partName, partTransform);
	TargettableBodyParts.Add(newTargettable);
}