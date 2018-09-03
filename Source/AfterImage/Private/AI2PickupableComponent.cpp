// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2PickupableComponent.h"


// Sets default values for this component's properties
UAI2PickupableComponent::UAI2PickupableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAI2PickupableComponent::BeginPlay()
{
	Super::BeginPlay();
	//Cast<AActor>(GetOwner())->OnActorBeginOverlap.AddDynamic(this, &UAI2PickupableComponent::Pickup);
	//ActorBeginOverlap.AddDynamic(this, &AAI2BaseNPC::FreezeNPC);
	//Cast<AAI2PlayerController>(GetWorld()->GetFirstPlayerController())->RecordStartedEvent.AddDynamic(this, &AAI2BaseNPC::FreezeNPC);
}


// Called every frame
void UAI2PickupableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
}

/*
void UAI2PickupableComponent::Pickup(AAI2Character* character) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, (TEXT("Pick Up")));
	
}

void UAI2PickupableComponent::ActorBeginOverlap(AActor* OtherActor) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, (TEXT("Overlap!!")));
}
*/