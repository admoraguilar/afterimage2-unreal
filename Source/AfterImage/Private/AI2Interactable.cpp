// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2Interactable.h"


// Sets default values
AAI2Interactable::AAI2Interactable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAI2Interactable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAI2Interactable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}