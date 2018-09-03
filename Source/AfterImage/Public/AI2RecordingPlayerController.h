// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2PlayerController.h"
#include "AI2Character.h"
//#include "AI2RecordingPlayerController.generated.h"
/*
//recording
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

class AAI2Character;

UCLASS()
class AFTERIMAGE_API AAI2RecordingPlayerController : public AAI2PlayerController
{
	GENERATED_BODY()
protected:
	TArray<FPawnInputData> RecordedInput;
	FPawnInputData* FindKeyRecord(FKey keyName);
	AAI2Character* InitialCharacter;
	AAI2Character* TempCharacter;
	bool bIsRecording = false;
	float timeElapsedAfterRecord;
	float RecordFrequency = 0.1f;
	float RecordFrequencyIndex;
	float replayFrequencyIndex;
	bool replaying;
	float replayTimeElapsed;
	void TestReplay(float deltaTime);
public:
	//constructor
	AAI2RecordingPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetupInputComponent() override;
	//input events
	virtual void OnRecordPressed();
	virtual void OnMoveForward(float p_val)override;
	virtual void OnMoveRight(float p_val)override;
	virtual void OnSprintPressed()override;
	virtual void OnJumpPressed()override;
	virtual void OnCrouchPressed()override;
	virtual void OnFirePressed()override;
	virtual void OnFireReleased()override;
	virtual void OnAimPressed()override;
	virtual void OnAimReleased()override;
	virtual void OnReloadPressed()override;
	virtual void OnCoverPressed()override;
	//recording
	void RecordInputData(FKey keyName, float relativeValueChanged, bool wasPressed);
	void RecordInputData(FKey keyName, bool isAxis, float relativeValueChanged, float axisValue);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class AAI2Character> EnactorCharacterClass;
};
*/