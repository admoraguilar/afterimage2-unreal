// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2BaseNPC.h"
#include "AI2PlayerController.h"
#include "AI2StateRestorationHandler.h"


// Sets default values
AAI2StateRestorationHandler::AAI2StateRestorationHandler() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAI2StateRestorationHandler::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AAI2StateRestorationHandler::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	if (GetWorld()->GetFirstPlayerController() && !bWasInitialized) {
		BindEvents();
		bWasInitialized = true;
	}
}

void AAI2StateRestorationHandler::BindEvents() {
	//wot de fuk
	/*
	Cast<AAI2PlayerController>(GetWorld()->GetFirstPlayerController())->RecordStartedEvent.RemoveDynamic(this, &AAI2StateRestorationHandler::SaveActorsData);
	Cast<AAI2PlayerController>(GetWorld()->GetFirstPlayerController())->RecordStartedEvent.AddDynamic(this, &AAI2StateRestorationHandler::SaveActorsData);

	Cast<AAI2PlayerController>(GetWorld()->GetFirstPlayerController())->CloningStartedEvent.RemoveDynamic(this, &AAI2StateRestorationHandler::RestoreActorsData);
	Cast<AAI2PlayerController>(GetWorld()->GetFirstPlayerController())->CloningStartedEvent.AddDynamic(this, &AAI2StateRestorationHandler::RestoreActorsData);
	*/
}

void AAI2StateRestorationHandler::SaveActorsData() {
	/*
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, (TEXT("saved actors data...")));
	ActorsToRestoreData.Empty();
	for (TActorIterator<AAI2BaseNPC> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		AAI2BaseNPC *tActor = *ActorItr;
		FTransform tTransform = tActor->GetActorTransform();
		bool tIsDead = false;
		FActorRestoreData* tActorData = new FActorRestoreData();
		tActorData->Actor = tActor;
		tActorData->Transform = tTransform;
		tActorData->bIsDead = tIsDead;
		ActorsToRestoreData.Add(tActorData);
	}*/
}

void AAI2StateRestorationHandler::RestoreActorsData() {
	/*
	if (ActorsToRestoreData.Num() > 0) {
		for (int i = 0; i < ActorsToRestoreData.Num() - 1; i++) {
			if (ActorsToRestoreData[i]->Actor != NULL) {
				ActorsToRestoreData[i]->Actor->SetActorTransform(ActorsToRestoreData[i]->Transform);
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, (TEXT("restored actors data...")));
			}
		}
	}*/
}