// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2GameMode.h"

AAI2GameMode::AAI2GameMode(const FObjectInitializer& ObjectInitializer) {
	
}

void AAI2GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);

	// Spawn custom systems
	for (int32 i = 0; i < CustomSystems.Num(); ++i) {
		GetWorld()->SpawnActor(CustomSystems[i]);
	}
}

void AAI2GameMode::BeginPlay() {
	FActorSpawnParameters SpawnInfo;
	//initialize game handlers
	if(TeamHandler==NULL) //if not yet initialized
		TeamHandler = GetWorld()->SpawnActor<AAI2TeamHandlerSingleton>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
	InteractionObserver = GetWorld()->SpawnActor<AAI2InteractionObserver>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
	StateRestorationHandler = GetWorld()->SpawnActor<AAI2StateRestorationHandler>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
}

AAI2TeamHandlerSingleton* AAI2GameMode::GetTeamHandler() {
	if (TeamHandler)
		return TeamHandler;
	else {
		FActorSpawnParameters SpawnInfo;
		TeamHandler = GetWorld()->SpawnActor<AAI2TeamHandlerSingleton>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
		return TeamHandler;
	}
}