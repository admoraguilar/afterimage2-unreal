// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#include "AfterImage.h"
#include "WActor.h"




void AWActor::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (GetWorld()) {
		ExecPreBeginPlay();
		PreBeginPlay();
	}
}

void AWActor::PreBeginPlay() {

}

void AWActor::ExecPreBeginPlay_Implementation() {

}
