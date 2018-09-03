// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#include "AfterImage.h"
#include "Statics/WDebugStatics.h"
#include "WStateMachine.h"




#pragma region UWObjectState

void UWObjectState::OnInit_Implementation() {}
void UWObjectState::OnEnter_Implementation() {}
void UWObjectState::OnExit_Implementation() {}
void UWObjectState::OnUpdate_Implementation() {}

void UWObjectState::Init(UObject* InObject) {
	Object = InObject;
}

UObject* UWObjectState::GetObject() const {
	return Object;
}

#pragma endregion




#pragma region UWDefaultState

void UWDefaultState::OnEnter_Implementation() {}
void UWDefaultState::OnExit_Implementation() {}
void UWDefaultState::OnUpdate_Implementation() {}

#pragma endregion




#pragma region UWStateMachine

UWStateMachine::UWStateMachine(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

UWObjectState* UWStateMachine::GetState() {
	return State.Num() > 0 ? State.Top() : NewObject<UWDefaultState>();
}

TArray<UWObjectState*> UWStateMachine::GetStates() {
	return State;
}

int32 UWStateMachine::GetStateCount() const {
	return State.Num();
}

bool UWStateMachine::IsState(TSubclassOf<class UWObjectState> InState) const {
	return State.Num() > 0 ? State.Top()->GetClass() == InState : false;
}

void UWStateMachine::PushState(TSubclassOf<class UWObjectState> InStateClass, UObject* InObject) {
	if (State.Num() > 0) {
		State.Top()->OnExit();
	}

	State.Push(NewObject<UWObjectState>(InObject, InStateClass));
	State.Top()->Init(InObject);
	State.Top()->OnInit();
	State.Top()->OnEnter();
	State.Top()->AddToRoot();
}

void UWStateMachine::SetState(TSubclassOf<class UWObjectState> InStateClass, UObject* InObject) {
	while (State.Num() > 0) {
		PopState();
	}

	PushState(InStateClass, InObject);
}

void UWStateMachine::PopState() {
	if (State.Num() > 0) {
		State.Top()->OnExit();
		State.Top()->RemoveFromRoot();
		State.Pop();
	}
	else {
		if (bDebugMode) QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("%s - is in no state.", *GetName());
	}

	if (State.Num() > 0) {
		State.Top()->OnEnter();
	}
}

void UWStateMachine::Init(TSubclassOf<UWObjectState> InState, UObject* InObject) {
	SetState(InState, InObject);
}

void UWStateMachine::DeInit() {
	if (bDebugMode) QUICK_ON_EDITOR_DEBUG_MESSAGE("Removing sticky states on state machine...");

	while (State.Num() > 0) {
		if (bDebugMode) QUICK_ON_EDITOR_DEBUG_MESSAGE_WITH_PARAMS("Removing - %s", *State.Top()->GetName());
		State.Top()->RemoveFromRoot();
		State.Pop();
	}
}

void UWStateMachine::Tick() {
	if (State.Num() > 0) {
		State.Top()->OnUpdate();
	}
}

void UWStateMachine::SetDebugMode(bool Value) {
	bDebugMode = Value;
}

#pragma endregion