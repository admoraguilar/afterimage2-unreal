// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2PlayerCamera.h"
#include "AI2RecordReceiverComponent.h"
#include "AI2RecordingPlayerController.h"
/*
AAI2RecordingPlayerController::AAI2RecordingPlayerController() {
	PlayerCameraManagerClass = AAI2PlayerCamera::StaticClass();
}

void AAI2RecordingPlayerController::BeginPlay() {
	InitialCharacter = GetControlledCharacter();
}

void AAI2RecordingPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AAI2RecordingPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAI2RecordingPlayerController::OnMoveRight);
	InputComponent->BindAxis("Turn", this, &AAI2PlayerController::OnTurn);
	InputComponent->BindAxis("LookUp", this, &AAI2PlayerController::OnLookUp);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AAI2RecordingPlayerController::OnSprintPressed);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AAI2RecordingPlayerController::OnJumpPressed);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AAI2RecordingPlayerController::OnCrouchPressed);
	InputComponent->BindAction("Fire", IE_Pressed, this, &AAI2RecordingPlayerController::OnFirePressed);
	InputComponent->BindAction("Fire", IE_Released, this, &AAI2RecordingPlayerController::OnFireReleased);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AAI2RecordingPlayerController::OnAimPressed);
	InputComponent->BindAction("Aim", IE_Released, this, &AAI2RecordingPlayerController::OnAimReleased);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AAI2RecordingPlayerController::OnReloadPressed);
	InputComponent->BindAction("Cover", IE_Pressed, this, &AAI2RecordingPlayerController::OnCoverPressed);
	InputComponent->BindAction("Record", IE_Pressed, this, &AAI2RecordingPlayerController::OnRecordPressed);
}

void AAI2RecordingPlayerController::Tick(float DeltaTime) {
	if (bIsRecording) {
		RecordFrequencyIndex += DeltaTime;
		if (RecordFrequencyIndex <= RecordFrequency) {
			timeElapsedAfterRecord += DeltaTime;
			RecordFrequencyIndex = 0.0f;
		}
	}

	if (replaying) {
		if (replayTimeElapsed < timeElapsedAfterRecord) {
			TestReplay(DeltaTime);
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, (TEXT("replaying: %s"), FString::SanitizeFloat(replayTimeElapsed)));
			replayTimeElapsed += DeltaTime;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, (TEXT("replay over")));
			TempCharacter->Destroy();
			replaying = false;
			replayFrequencyIndex = 0;
			timeElapsedAfterRecord = 0.0f;
		}
	}
}

FPawnInputData* AAI2RecordingPlayerController::FindKeyRecord(FKey keyName) {
	//if array still empty, then key doesnt exist
	if (RecordedInput.Num() == 0)
		return nullptr;
	for (int i = 0; i < RecordedInput.Num(); i++) {
		if (RecordedInput[i].KeyName == keyName) {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, (TEXT("Found key record!")));
			return &RecordedInput[i];
		}
	}
	//if nothing was found, key doesnt exist
	return nullptr;
}

void AAI2RecordingPlayerController::RecordInputData(FKey keyName, float relativeValueChanged, bool wasPressed) {
	FPawnInputData* queryCache = FindKeyRecord(keyName);
	//if key never been pressed yet, create a new record
	if (queryCache == nullptr) {
		FPawnInputData keyEntry;
		keyEntry.KeyName = keyName;
		keyEntry.ValueChangeOnTime.Add(relativeValueChanged);
		keyEntry.WasPressed.Add(wasPressed);
		RecordedInput.Add(keyEntry);
	}
	//if key has been recorded, just add data to key record
	else {
		queryCache->ValueChangeOnTime.Add(relativeValueChanged);
		queryCache->WasPressed.Add(wasPressed);
	}
}


void AAI2RecordingPlayerController::RecordInputData(FKey keyName, bool isAxis, float relativeValueChanged, float axisValue) {
	FPawnInputData* queryCache = FindKeyRecord(keyName);
	//if key never been pressed yet, create a new record
	if (queryCache == nullptr) {
		FPawnInputData keyEntry;
		keyEntry.KeyName = keyName;
		keyEntry.IsAxisKey = isAxis;
		keyEntry.ValueChangeOnTime.Add(relativeValueChanged);
		keyEntry.AxisValue.Add(axisValue);
		RecordedInput.Add(keyEntry);
	}
	//if key has been recorded, just add data to key record
	else {
		queryCache->ValueChangeOnTime.Add(relativeValueChanged);
		queryCache->AxisValue.Add(axisValue);
	}
}

void AAI2RecordingPlayerController::OnMoveForward(float p_val) {
	if (p_val != 0) {
		AAI2PlayerController::OnMoveForward(p_val);
		if (bIsRecording) {
			if (p_val > 0) 
				RecordInputData(EKeys::W, timeElapsedAfterRecord, true);
			else
				RecordInputData(EKeys::S, timeElapsedAfterRecord, true);
		}
	}
}

void AAI2RecordingPlayerController::OnMoveRight(float p_val) {
	if (p_val != 0) {
		AAI2PlayerController::OnMoveRight(p_val);
		if (bIsRecording) {
			if (p_val > 0) 
				RecordInputData(EKeys::D, timeElapsedAfterRecord, true);
			else
				RecordInputData(EKeys::A, timeElapsedAfterRecord, true);
		}
	}
}

void AAI2RecordingPlayerController::OnSprintPressed() {
	AAI2PlayerController::OnSprintPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::LeftShift, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnJumpPressed() {
	AAI2PlayerController::OnJumpPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::SpaceBar, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnCrouchPressed() {
	AAI2PlayerController::OnCrouchPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::C, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnFirePressed() {
	AAI2PlayerController::OnFirePressed();
	if (bIsRecording) {
		RecordInputData(EKeys::LeftMouseButton, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnFireReleased() {
	AAI2PlayerController::OnFireReleased();
	if (bIsRecording) {
		RecordInputData(EKeys::LeftMouseButton, timeElapsedAfterRecord, false);
	}
}


void AAI2RecordingPlayerController::OnAimPressed() {
	AAI2PlayerController::OnAimPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::RightMouseButton, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnAimReleased() {
	AAI2PlayerController::OnAimReleased();
	if (bIsRecording) {
		RecordInputData(EKeys::RightMouseButton, timeElapsedAfterRecord, false);
	}
}

void AAI2RecordingPlayerController::OnReloadPressed() {
	AAI2PlayerController::OnReloadPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::R, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnCoverPressed() {
	AAI2PlayerController::OnCoverPressed();
	if (bIsRecording) {
		RecordInputData(EKeys::F, timeElapsedAfterRecord, true);
	}
}

void AAI2RecordingPlayerController::OnRecordPressed() {
	switch (bIsRecording) {
		case true:
			replayTimeElapsed = 0;
			bIsRecording = false;
			replaying = true;
			//reset enactor transform to initial transform
			TempCharacter->SetActorTransform(InitialCharacter->GetActorTransform());
			Possess(InitialCharacter);
			break;
		case false:
			RecordedInput.Empty();
			timeElapsedAfterRecord = 0.0f;
			bIsRecording = true;
			FVector spawnLoc = GetPawn()->GetActorLocation();
			FRotator spawnRot = GetPawn()->GetActorRotation();
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AGameMode* const gm = GetWorld()->GetAuthGameMode<AGameMode>();
			TempCharacter = GetWorld()->SpawnActor<AAI2Character>(gm->DefaultPawnClass, spawnLoc, spawnRot, SpawnInfo);
			Possess(TempCharacter);
			break;
	}
}


void AAI2RecordingPlayerController::TestReplay(float deltaTime) {
	//test translator logic
	for (int i = 0; i < RecordedInput.Num(); i++) {
		for (int j = 0; j < RecordedInput[i].ValueChangeOnTime.Num(); j++) {
			float toleranceVal = 0.1f;
			if ((RecordedInput[i].ValueChangeOnTime[j] >= (replayTimeElapsed - toleranceVal))
				&& (RecordedInput[i].ValueChangeOnTime[j] <= (replayTimeElapsed + toleranceVal))) {  //if matches deltaTime within a tolerance value

				//test vert
				if ((RecordedInput[i].KeyName == EKeys::W)) {
					OnMoveForward(RecordedInput[i].AxisValue[j]);
				}
				//test hor
				if ((RecordedInput[i].KeyName == EKeys::S)) {
					OnMoveRight(RecordedInput[i].AxisValue[j]);
				}
				//legit 
				if ((RecordedInput[i].KeyName == EKeys::LeftShift) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->InterpretKey(EKeys::LeftShift);
				}
				if ((RecordedInput[i].KeyName == EKeys::SpaceBar) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->InterpretKey(EKeys::SpaceBar);
				}
				if ((RecordedInput[i].KeyName == EKeys::RightMouseButton) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->Aim();
				}
				if ((RecordedInput[i].KeyName == EKeys::RightMouseButton) && !(RecordedInput[i].WasPressed[j])) {
					TempCharacter->StopAiming();
				}
				if ((RecordedInput[i].KeyName == EKeys::R) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->InterpretKey(EKeys::R);
				}
				if ((RecordedInput[i].KeyName == EKeys::LeftMouseButton) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->Fire();
				}
				if ((RecordedInput[i].KeyName == EKeys::LeftMouseButton) && !(RecordedInput[i].WasPressed[j])) {
					TempCharacter->StopFiring();
				}
				if ((RecordedInput[i].KeyName == EKeys::C) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->InterpretKey(EKeys::C);
				}
				if ((RecordedInput[i].KeyName == EKeys::F) && (RecordedInput[i].WasPressed[j])) {
					TempCharacter->InterpretKey(EKeys::F);
				}
				GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, (TEXT("Replaying...")));
			}
		}
	}
}
*/