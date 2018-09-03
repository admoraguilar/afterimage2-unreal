// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "Public/Core/Object/WObject.h"
#include "WEventSystem.generated.h"




DECLARE_DYNAMIC_DELEGATE_OneParam(FGameEventDelegate, UWGameEventDetails*, GameEventDetails);
DECLARE_MULTICAST_DELEGATE_OneParam(FGameEventMulticastDelegate, UWGameEventDetails*);




UENUM(BlueprintType)
enum class EGameEventName : uint8 {
	Default,
	Game_EnableInput,
	Game_DisableInput,
	Menu_StartOnClicked,
	Menu_SettingsOnClicked,
	Menu_CreditsOnClicked,
	Menu_QuitClicked,
	Menu_BackToMainClicked,
};




/**
*
*/
UCLASS(Blueprintable)
class AFTERIMAGE_API UWGameEventDetails : public UWObject {
	GENERATED_BODY()

protected:
	UWGameEventDetails(const class FObjectInitializer& ObjectInitializer);
};




/**
*
*/
UCLASS()
class AFTERIMAGE_API UWEventSystem : public UWObject {
	GENERATED_BODY()

private:
	static TMap<EGameEventName, FGameEventMulticastDelegate> _EnumDelegates;
	static TMap<FString, FGameEventMulticastDelegate> _StringDelegates;

public:
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods") static void AddListener(EGameEventName EventName, FGameEventDelegate GameEvent);
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods") static void RemoveListener(EGameEventName EventName, FGameEventDelegate GameEvent);
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods", meta = (AdvancedDisplay = "1")) static void RaiseEvent(EGameEventName EventName, UWGameEventDetails* GameEventDetails = nullptr);
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods") static void AddListenerString(FString EventName, FGameEventDelegate GameEvent);
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods") static void RemoveListenerString(FString EventName, FGameEventDelegate GameEvent);
	UFUNCTION(BlueprintCallable, Category = "WEventSystem|Methods", meta = (AdvancedDisplay = "1")) static void RaiseEventString(FString EventName, UWGameEventDetails* GameEventDetails = nullptr);

	template<class T> static void AddListener(EGameEventName EventName, T* UserObject, FName FunctionName, void (T::* FunctionPtr)(UWGameEventDetails*)) {
		FGameEventMulticastDelegate* D = NULL;

		if (_EnumDelegates.Contains(EventName)) {
			D = &_EnumDelegates[EventName];
		}
		else {
			_EnumDelegates.Add(EventName);
			D = &_EnumDelegates[EventName];
		}

		//D->__Internal_AddDynamic(UserObject, FunctionPtr, FunctionName);
		D->AddUFunction(UserObject, FunctionName);
	}
	template<class T> static void RemoveListener(EGameEventName EventName, T* UserObject, FName FunctionName, void (T::* FunctionPtr)(UWGameEventDetails*)) {
		FGameEventMulticastDelegate* D = NULL;

		if (_EnumDelegates.Contains(EventName)) {
			D = &_EnumDelegates[EventName];
		}
		else {
			//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Event to remove is non-existent] = %s", *UWDebugStatics::GetEnumAsString<EGameEventName>("EGameEventName", EventName));
			return;
		}

		//D->__Internal_RemoveDynamic(UserObject, FunctionPtr, FunctionName);
		D->AddUFunction(UserObject, FunctionName);
	}
	template<class T> static void RaiseEvent(EGameEventName EventName, T* GameEventDetails) {
		//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Raising event] = %s", *UWDebugStatics::GetEnumAsString<EGameEventName>("EGameEventName", EventName));

		if (_EnumDelegates.Contains(EventName)) {
			_EnumDelegates[EventName].Broadcast(GameEventDetails);
		}
		else {
			//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Raised event is non existent] = %s", *UWDebugStatics::GetEnumAsString<EGameEventName>("EGameEventName", EventName));
		}
	}
	template<class T> static void AddListenerString(FString EventName, T* UserObject, FName FunctionName, void (T::* FunctionPtr)(UWGameEventDetails*)) {
		FGameEventMulticastDelegate* D = NULL;

		if (_StringDelegates.Contains(EventName)) {
			D = &_StringDelegates[EventName];
		}
		else {
			_StringDelegates.Add(EventName);
			D = &_StringDelegates[EventName];
		}

		//D->__Internal_AddDynamic(UserObject, FunctionPtr, FunctionName);
		D->AddUFunction(UserObject, FunctionName);
	}
	template<class T> static void RemoveListenerString(FString EventName, T* UserObject, FName FunctionName, void (T::* FunctionPtr)(UWGameEventDetails*)) {
		FGameEventMulticastDelegate* D = NULL;

		if (_StringDelegates.Contains(EventName)) {
			D = &_StringDelegates[EventName];
		}
		else {
			//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Event to remove is non-existent] = %s", *EventName);
			return;
		}

		//D->__Internal_RemoveDynamic(UserObject, FunctionPtr, FunctionName);
		D->AddUFunction(UserObject, FunctionName);
	}
	template<class T> static void RaiseEventString(FString EventName, T* GameEventDetails) {
		//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Raising event] = %s", *EventName);

		if (_StringDelegates.Contains(EventName)) {
			_StringDelegates[EventName].Broadcast(GameEventDetails);
		}
		else {
			//QUICK_ON_SCREEN_DEBUG_MESSAGE_WITH_PARAMS("[Raised event is non existent] = %s", *EventName);
		}
	}
};
