// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#include "AfterImage.h"
#include "Statics/WDebugStatics.h"
#include "WSequenceSystem.h"




TArray<class AWSequenceSystem*> AWSequenceSystem::Instances;

AWSequenceSystem::AWSequenceSystem() {
	PrimaryActorTick.bCanEverTick = true;
}

void AWSequenceSystem::PreBeginPlay() {
	Super::PreBeginPlay();

	// Store instance
	if (!Instances.Contains(this)) Instances.Add(this);

	// Initialize systems
	SequenceState = NewObject<UWStateMachine>();
	SequenceState->Init(UWSequenceState::StaticClass(), this);
}

void AWSequenceSystem::EndPlay(EEndPlayReason::Type InEndPlayReason) {
	Super::EndPlay(InEndPlayReason);

	// Removing references
	if (Instances.Contains(this)) Instances.Remove(this);
	SequenceState->DeInit();
}

void AWSequenceSystem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	SequenceState->Tick();
}

AWSequenceSystem* AWSequenceSystem::GetSequenceSystem(ESequenceSystemName Name) {
	AWSequenceSystem* MS = nullptr;

	for (int i = 0; i < Instances.Num(); ++i) {
		if (Instances[i]->SequenceSystemName == Name) {
			MS = Instances[i];
			break;
		}
	}

	return MS;
}

void AWSequenceSystem::SetSequence(TSubclassOf<class UWSequenceState> InState) {
	SequenceState->SetState(InState, this);
}

bool AWSequenceSystem::IsSequence(TSubclassOf<class UWSequenceState> InState) const {
	return SequenceState->IsState(InState);
}

void AWSequenceSystem::PushSequence(TSubclassOf<class UWSequenceState> InState) {
	SequenceState->PushState(InState, this);
}

void AWSequenceSystem::PopSequence() {
	SequenceState->PopState();
}