// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2SpectatorCharacter.h"

AAI2SpectatorCharacter::AAI2SpectatorCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) {
	//SetCharacterState<USpectator_Move>();
	//CameraComponent->AddOrUpdateBlendable(
}

void AAI2SpectatorCharacter::BeginPlay() {
	Super::BeginPlay();
	//Initialize State
	CurrentState = NewObject<USpectator_Move>(); 
	CurrentState->AddToRoot();
}


void USpectator_Move::HandleInput(AAI2Character* character, FKey input) {
	if (input == EKeys::W)
		character->MoveForward(1);
	else if (input == EKeys::S)
		character->MoveForward(-1);
	else if (input == EKeys::D)
		character->MoveRight(1);
	else if (input == EKeys::A)
		character->MoveRight(-1);
}