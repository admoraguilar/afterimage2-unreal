// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2PlayerController.h"
#include "AI2BaseNPC.h"

class AAI2PlayerController;
// Sets default values
AAI2BaseNPC::AAI2BaseNPC() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAI2BaseNPC::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AAI2BaseNPC::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
}

// Called to bind functionality to input
void AAI2BaseNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}