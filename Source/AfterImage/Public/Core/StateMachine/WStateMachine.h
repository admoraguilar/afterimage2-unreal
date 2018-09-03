// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "Core/Object/WObject.h"
#include "WStateMachine.generated.h"




/**
 * 
 */
UCLASS(ABSTRACT, Blueprintable, BlueprintType)
class AFTERIMAGE_API UWObjectState : public UWObject {
	GENERATED_BODY()

	class UObject* Object;
	
public:
	UFUNCTION(BlueprintNativeEvent, Category="WStateMachine|WObjectState|Methods") void OnInit();
	UFUNCTION(BlueprintNativeEvent, Category="WStateMachine|WObjectState|Methods") void OnEnter();
	UFUNCTION(BlueprintNativeEvent, Category="WStateMachine|WObjectState|Methods") void OnExit();
	UFUNCTION(BlueprintNativeEvent, Category="WStateMachine|WObjectState|Methods") void OnUpdate();
	virtual void OnInit_Implementation();
	virtual void OnEnter_Implementation();
	virtual void OnExit_Implementation();
	virtual void OnUpdate_Implementation();

	UFUNCTION(BlueprintCallable, Category="WStateMachine|WObjectState|Methods") UObject* GetObject() const;

	void Init(UObject* InObject);

	template<class T> T* GetObject() {
		return Cast<T>(Object);
	}
};




/**
*
*/
UCLASS()
class AFTERIMAGE_API UWDefaultState : public UWObjectState {
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;
	virtual void OnUpdate_Implementation() override;
};




/**
 * 
 */
UCLASS()
class AFTERIMAGE_API UWStateMachine : public UWObject {
	GENERATED_BODY()

	TArray<class UWObjectState*> State;
	bool bDebugMode = true;

public:
	UWStateMachine(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") class UWObjectState* GetState();
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") TArray<class UWObjectState*> GetStates(); 
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") int32 GetStateCount() const;
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") bool IsState(TSubclassOf<class UWObjectState> InState) const;
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") void PushState(TSubclassOf<class UWObjectState> InStateClass, UObject* InObject);
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") void SetState(TSubclassOf<class UWObjectState> InStateClass, UObject* InObject);
	UFUNCTION(BlueprintCallable, Category="WStateMachine|Methods") void PopState();

	void Init(TSubclassOf<UWObjectState> InState, UObject* InObject);
	void DeInit();
	void Tick();
	void SetDebugMode(bool Value);
	
	template<class T> bool IsState() {
		return State.Num() > 0 ? State.Top()->GetClass() == NewObject<T>()->GetClass() : false;
	}
	template<class T> void PushState(UObject* InObject) {
		if(State.Num() > 0) {
			State.Top()->OnExit();
		}

		State.Push(NewObject<T>(InObject));
		State.Top()->Init(InObject);
		State.Top()->OnInit();
		State.Top()->OnEnter();
		State.Top()->AddToRoot();
	}
	template<class T> void SetState(UObject* InObject) {
		while(State.Num() > 0) {
			PopState();
		}

		PushState<T>(InObject);
	}
};
