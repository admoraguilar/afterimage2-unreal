// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "EngineUtils.h"
#include "AI2InteractionObserver.h"


// Sets default values
AAI2InteractionObserver::AAI2InteractionObserver() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAI2InteractionObserver::BeginPlay() {
	Super::BeginPlay();
	//get all interactables in world and add them to reference array
	GetInteractableReferences();
}

// Called every frame
void AAI2InteractionObserver::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, (TEXT("%s"), FString::FromInt(InteractableObjects.Num())));
}

AAI2Interactable* AAI2InteractionObserver::RequestForNearbyInteractable(AAI2Character* querier) {
	AAI2Interactable* nearestInteractable = nullptr;
	float nearestDistance = 999999999999;
	for (int i = 0; i < InteractableObjects.Num(); i++) {
		//get distance between interactor and interactable
		float tDist = FVector::Dist(querier->GetActorLocation(), InteractableObjects[i]->GetActorLocation());
		//is it close enough?
		if (tDist <= INTERACT_MINIMUM_DISTANCE) {
			//is it closer than the last nearest interactable?
			if (tDist < nearestDistance) {
				nearestInteractable = InteractableObjects[i];
				nearestDistance = tDist;
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, (TEXT("FINISHED REQUEST")));
	return nearestInteractable;
}

void AAI2InteractionObserver::GetInteractableReferences() {
	InteractableObjects.Empty();
	for (TActorIterator<AAI2Interactable> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AAI2Interactable *tInteractableInWorld = *ActorItr;
		InteractableObjects.Add(tInteractableInWorld);
	}
}