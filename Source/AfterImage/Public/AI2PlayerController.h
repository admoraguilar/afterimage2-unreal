// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2Character.h"
#include "GameFramework/PlayerController.h"
#include "AI2PlayerController.generated.h"

class AAI2Character;

USTRUCT()
struct FPawnInputData {
	GENERATED_USTRUCT_BODY()
		UPROPERTY()
			FKey KeyName;
		UPROPERTY()
			bool IsAxisKey = false;
		UPROPERTY()
			TArray<float> AxisValue;
		UPROPERTY()
			TArray<float> ValueChangeOnTime;
		UPROPERTY()
			TArray<bool> WasPressed;
		FPawnInputData() {}
};

USTRUCT()
struct FCloneSlotData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
		AAI2Character* Character;
	UPROPERTY()
		TArray<FPawnInputData> InputData;
	UPROPERTY()
		TArray<FTransform> TransformData;
	UPROPERTY()
		TArray<FRotator> ControlRotData;
	int TransformIndex = 0;
	int CtrlRotIndex = 0;
	bool bIsReplaying = false;
	//constructors
	FCloneSlotData() {}
	FCloneSlotData(AAI2Character* character) {
		Character = character;
	}
	//functions
	void SetCloneActive(bool active);
	void ClearData();
};

UENUM(BlueprintType)
enum class EControlState : uint8 {
	CS_Normal,
	CS_Enactor,
	CS_Rewind
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerControllerEvent);

UCLASS()
class AFTERIMAGE_API AAI2PlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	AAI2Character* GetControlledCharacter(){return Cast<AAI2Character>(GetPawn()); }
	//separate enactor character needed for VFX 
	AAI2Character* RecordEnactorCharacter;
	TArray<FPawnInputData> RecordedInput;
	const int MAXSPAWNABLECLONES = 4;
	UPROPERTY(BlueprintReadOnly, Category = "AI2PlayerController")  int ActiveCloneIndex = 0;
	//Clones object pool w/ recorded data
	TArray<FCloneSlotData*> CloneDataInventory;
	float timeElapsedAfterRecord;
	FPawnInputData* FindKeyRecord(FKey keyName);
	float replayFrequencyIndex;
	bool replaying;
	float replayTimeElapsed;
	//"original" character, main character at start of game
	AAI2Character* InitialCharacter;
	//target for possessing mechanic, could be used for more
	APawn* Target;
	bool bIsRecordPressed = false;
	//recording related
	float RecordHoldTime = 0;
	//hold time required for a press to be considered a "hold"
	const float RECORDHOLDTIMEREQUIRED = 0.3f;
	//functions
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//test functions
	void TestReplay(FCloneSlotData* target, float deltaTime);
	void RecordInputData(FKey keyName, float relativeValueChanged, bool wasPressed);
	void RecordInputData(FKey keyName, bool isAxis, float relativeValueChanged, float axisValue);
	AAI2Character* TraceForClone();
	float RecordFrequency = 0.1f;
	float RecordFrequencyIndex;

	int numberOfReplayingClones = 0;

public:
	UPROPERTY(BlueprintReadWrite, Category = "AI2PlayerController")
		float CurRecordTimePool = 10.f;
	//max record time pool
	UPROPERTY(BlueprintReadWrite, Category = "AI2PlayerController")
		float MAX_RECORDTIMEPOOL = 10.f;
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		inline float GetCurrentRecordPool() {
			return CurRecordTimePool;
		}	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		inline float GetRecordTimeElapsed() {
			return timeElapsedAfterRecord;
	}
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		inline float GetNumberOfActiveClones() {
		return numberOfReplayingClones;
	}
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay")
		EControlState CurrentControlState;
	//Events
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent AnyKeyPressedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent RecordStartedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent RecordEndedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent FreecamStartedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent EnactorStartedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent CloningStartedEvent;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FPlayerControllerEvent CloningEndedEvent;
	//constructor
	AAI2PlayerController();
	//setup input
	void SetupInputComponent() override;
	virtual void OnMoveForward(float p_val);
	virtual void OnMoveRight(float p_val);
	virtual void OnTurn(float p_val);
	virtual void OnLookUp(float p_val);
	virtual void OnSprintPressed();
	virtual void OnJumpPressed();
	virtual void OnCrouchPressed();
	virtual void OnFirePressed();
	virtual void OnFireReleased();
	virtual void OnAimPressed();
	virtual void OnAimReleased();
	virtual void OnReloadPressed();
	virtual void OnCoverPressed();
	virtual void OnRecordPressed();
	virtual void OnRecordReleased();
	virtual void OnInteractPressed();
	virtual void OnInteractReleased();
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void OnEnactorHasDied();
	
	UPROPERTY(BlueprintReadWrite, Category = "AI2PlayerController")
		float MAXRECORDTIME = 99999.0f;
	void LerpCameraTo(AActor* Actor);
	void ActivateCloneReplay();
	void DeactivateCloneReplay();
	void ClearRecordedData();
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void ToggleRecord();
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void StopRecording();
	UFUNCTION(BlueprintCallable, Category = "AI2PlayerController")
		AAI2Character* GetInitialCharacter();
	UPROPERTY(BlueprintReadWrite, Category = "AI2PlayerController")
		bool bIsRecordingDisabled = false;
	//if the player gets a gun, give all (reserved) clones guns kek
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void UpdateClonelistWeapon(AAI2Character* Acquirer);
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void SetRewindBackToNormal();
};
