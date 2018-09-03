// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#include "AfterImage.h"
#include "Public/Statics/WDebugStatics.h"
#include "WEventSystem.h"




/**
*
*/
UWGameEventDetails::UWGameEventDetails(const class FObjectInitializer& ObjectInitializer) {

}




/**
*
*/
TMap<EGameEventName, FGameEventMulticastDelegate> UWEventSystem::_EnumDelegates;
TMap<FString, FGameEventMulticastDelegate> UWEventSystem::_StringDelegates;

void UWEventSystem::AddListener(EGameEventName EventName, FGameEventDelegate GameEvent) {
	UWEventSystem::AddListener<UObject>(EventName, GameEvent.GetUObject(), GameEvent.GetFunctionName(), nullptr);
}

void UWEventSystem::RemoveListener(EGameEventName EventName, FGameEventDelegate GameEvent) {
	UWEventSystem::RemoveListener<UObject>(EventName, GameEvent.GetUObject(), GameEvent.GetFunctionName(), nullptr);
}

void UWEventSystem::RaiseEvent(EGameEventName EventName, UWGameEventDetails* GameEventDetails) {
	UWEventSystem::RaiseEvent<UWGameEventDetails>(EventName, !GameEventDetails ? NewObject<UWGameEventDetails>(UWGameEventDetails::StaticClass()) : GameEventDetails);
}

void UWEventSystem::AddListenerString(FString EventName, FGameEventDelegate GameEvent) {
	UWEventSystem::AddListenerString<UObject>(EventName, GameEvent.GetUObject(), GameEvent.GetFunctionName(), nullptr);
}

void UWEventSystem::RemoveListenerString(FString EventName, FGameEventDelegate GameEvent) {
	UWEventSystem::RemoveListenerString<UObject>(EventName, GameEvent.GetUObject(), GameEvent.GetFunctionName(), nullptr);
}

void UWEventSystem::RaiseEventString(FString EventName, UWGameEventDetails* GameEventDetails) {
	UWEventSystem::RaiseEventString<UWGameEventDetails>(EventName, !GameEventDetails ? NewObject<UWGameEventDetails>(UWGameEventDetails::StaticClass()) : GameEventDetails);
}