// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#include "AfterImage.h"
#include "Statics/WDebugStatics.h"
#include "WMenuSystem.h"




#pragma region UWMenuManager

TArray<class AWMenuSystem*> AWMenuSystem::Instances;

AWMenuSystem::AWMenuSystem() {
	PrimaryActorTick.bCanEverTick = true;
}

void AWMenuSystem::PreBeginPlay() {
	Super::PreBeginPlay();

	// Store instance
	if (!Instances.Contains(this)) Instances.Add(this);

	//// Initialize systems
	MenuState = NewObject<UWStateMachine>();
	MenuState->Init(UWMenuState::StaticClass(), this);
}

void AWMenuSystem::EndPlay(EEndPlayReason::Type InEndPlayReason) {
	Super::EndPlay(InEndPlayReason);

	// Removing references
	if (Instances.Contains(this)) Instances.Remove(this);
	MenuState->DeInit();
}

void AWMenuSystem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	MenuState->Tick();

	/*if(MenuState->GetStateCount() == 1 && MenuState->IsState(UAI2MenuState::StaticClass())) {
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = 0;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetMouseLocation(UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X / 2, UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y / 2);
	}*/
}

AWMenuSystem* AWMenuSystem::GetMenuSystem(EMenuSystemName Name) {
	AWMenuSystem* MS = nullptr;

	for (int i = 0; i < Instances.Num(); ++i) {
		if (Instances[i]->MenuSystemName == Name) {
			MS = Instances[i];
			break;
		}
	}

	return MS;
}

void AWMenuSystem::SetMenu(TSubclassOf<UWMenuState> InState) {
	MenuState->SetState(InState, this);
}

bool AWMenuSystem::IsMenu(TSubclassOf<class UWMenuState> InState) const {
	return MenuState->IsState(InState);
	return false;
}

void AWMenuSystem::PushMenu(TSubclassOf<class UWMenuState> InState) {
	MenuState->PushState(InState, this);
}

void AWMenuSystem::PopMenu() {
	MenuState->PopState();
}

#pragma endregion

