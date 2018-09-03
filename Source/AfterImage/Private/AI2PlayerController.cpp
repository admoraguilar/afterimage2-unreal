// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2StatsComponent.h"
#include "AI2PlayerCamera.h"
#include "AI2PlayerController.h"

//Clone Data Functions
void FCloneSlotData::ClearData() {
	bIsReplaying = false;
	InputData.Empty();
	TransformData.Empty();
	ControlRotData.Empty();
	TransformIndex = 0;
	CtrlRotIndex = 0;
}

void FCloneSlotData::SetCloneActive(bool active) {
	if (!active) {
		Character->SetActorHiddenInGame(true);
		Character->SetActorEnableCollision(false);
		Character->PrimitiveIndicatorDebug->SetHiddenInGame(true);
	}
	else {
		Character->SetActorHiddenInGame(false);
		Character->SetActorEnableCollision(true);
	}
}

//Game Controller Implementations
AAI2PlayerController::AAI2PlayerController() {
	PlayerCameraManagerClass = AAI2PlayerCamera::StaticClass();
}

void AAI2PlayerController::BeginPlay() {
	//default controller phase
	CurrentControlState = EControlState::CS_Normal;
	//init record time pool
	CurRecordTimePool = MAX_RECORDTIMEPOOL;
	//setup init character and bind events
	InitialCharacter = GetControlledCharacter();
	InitialCharacter->OnAcquireWeapon.AddDynamic(this, &AAI2PlayerController::UpdateClonelistWeapon);
	//setup extra actors
	AAI2GameMode* const gm = GetWorld()->GetAuthGameMode<AAI2GameMode>();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//Initialize object pool 
	for (int i = 0; i < MAXSPAWNABLECLONES; i++) {
		//spawn character clones
		AAI2Character* temp = GetWorld()->SpawnActor<AAI2Character>(gm->EnactorCharacterClass, GetPawn()->GetActorLocation(),
			GetPawn()->GetActorRotation(),
			SpawnInfo);
		//we need a controller for our character clones or else replay wont work
		temp->SpawnDefaultController();
		//hide and disable collision
		temp->SetActorHiddenInGame(true);
		temp->SetActorEnableCollision(false);
		FCloneSlotData* tempSlot = new FCloneSlotData(temp);
		CloneDataInventory.Add(tempSlot);
	}
	//init enactor character
	RecordEnactorCharacter = GetWorld()->SpawnActor<AAI2Character>(gm->EnactorCharacterClass, GetPawn()->GetActorLocation(),
		GetPawn()->GetActorRotation(),
		SpawnInfo);
	if(RecordEnactorCharacter->CharacterStatsComponent)
		RecordEnactorCharacter->CharacterStatsComponent->OnHealthDroppedZeroEvent.AddDynamic(this, &AAI2PlayerController::OnEnactorHasDied);
	//hide on start
	RecordEnactorCharacter->SetActorHiddenInGame(true);
	RecordEnactorCharacter->SetActorEnableCollision(false);
}

void AAI2PlayerController::OnEnactorHasDied() {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, (TEXT("event called tho!!!!")));
	if (CurrentControlState == EControlState::CS_Enactor) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, (TEXT("ENACTOR DIED!!!!")));
		//disable inputs
		DisableInput(this);
		//back to normal phase
		CurrentControlState = EControlState::CS_Rewind;
		//Fire gameplay events
		CloningStartedEvent.Broadcast();
		RecordEndedEvent.Broadcast();
	}
}

void AAI2PlayerController::UpdateClonelistWeapon(AAI2Character* Acquirer) {
	if (Acquirer == GetControlledCharacter()) {
		for(int i = 0; i < CloneDataInventory.Num(); i++)
			CloneDataInventory[i]->Character->EquipWeapon();
	}
	RecordEnactorCharacter->EquipWeapon();
}

void AAI2PlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	
	InputComponent->BindAxis("MoveForward", this, &AAI2PlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAI2PlayerController::OnMoveRight);
	InputComponent->BindAxis("Turn", this, &AAI2PlayerController::OnTurn);
	InputComponent->BindAxis("LookUp", this, &AAI2PlayerController::OnLookUp);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AAI2PlayerController::OnSprintPressed);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AAI2PlayerController::OnCrouchPressed);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AAI2PlayerController::OnFirePressed);
	InputComponent->BindAction("Fire", IE_Released, this, &AAI2PlayerController::OnFireReleased);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AAI2PlayerController::OnAimPressed);
	InputComponent->BindAction("Aim", IE_Released, this, &AAI2PlayerController::OnAimReleased);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AAI2PlayerController::OnReloadPressed);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AAI2PlayerController::OnInteractPressed);
	InputComponent->BindAction("Interact", IE_Released, this, &AAI2PlayerController::OnInteractReleased);

	InputComponent->BindAction("Record", IE_Pressed, this, &AAI2PlayerController::OnRecordPressed);
	InputComponent->BindAction("Record", IE_Released, this, &AAI2PlayerController::OnRecordReleased);
}

void AAI2PlayerController::StopRecording() {
	DisableInput(this);
	//back to normal phase
	CurrentControlState = EControlState::CS_Rewind;
	CloningStartedEvent.Broadcast();
	RecordEndedEvent.Broadcast();
}

AAI2Character* AAI2PlayerController::GetInitialCharacter() {
	return InitialCharacter;
}

AAI2Character* AAI2PlayerController::TraceForClone() {
	const FName TraceTag("ControllerTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("ControllerTag")), true, this);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	TraceParams.TraceTag = TraceTag;
	GetWorld()->DebugDrawTraceTag = TraceTag;

	FVector traceStart = Cast<AAI2Character>(GetPawn())->GetCameraComponent()->GetComponentLocation();
	FVector traceEnd = traceStart + Cast<AAI2Character>(GetPawn())->GetCameraComponent()->GetForwardVector() * 99999999;
	
	GetWorld()->LineTraceSingleByChannel(
		HitData,				//result
		traceStart,	//start
		traceEnd,		//end
		ECC_GameTraceChannel2,
		TraceParams
	);

	AGameMode* const gm = GetWorld()->GetAuthGameMode<AGameMode>();
	if (HitData.Actor != NULL) {
		if (HitData.Actor->GetClass() == gm->DefaultPawnClass)
			return Cast<AAI2Character>(HitData.Actor.Get());
	}
	return nullptr;
}

FPawnInputData* AAI2PlayerController::FindKeyRecord(FKey keyName) {
	//if array still empty, then key doesnt exist
	if (CloneDataInventory[ActiveCloneIndex]->InputData.Num() == 0)
		return nullptr;
	for (int i = 0; i < CloneDataInventory[ActiveCloneIndex]->InputData.Num(); i++) {
		if (CloneDataInventory[ActiveCloneIndex]->InputData[i].KeyName == keyName) {
			return &CloneDataInventory[ActiveCloneIndex]->InputData[i];
		}
	}
	//if nothing was found, key doesnt exist
	return nullptr;
}

void AAI2PlayerController::RecordInputData(FKey keyName, float relativeValueChanged, bool wasPressed) {
	FPawnInputData* queryCache = FindKeyRecord("keyName");
	//if key never been pressed yet, create a new record
	if (queryCache == nullptr) {
		FPawnInputData keyEntry;
		keyEntry.KeyName = keyName;
		keyEntry.ValueChangeOnTime.Add(relativeValueChanged);
		keyEntry.WasPressed.Add(wasPressed);
		CloneDataInventory[ActiveCloneIndex]->InputData.Add(keyEntry);
	}
	//if key has been recorded, just add data to key record
	else {
		queryCache->ValueChangeOnTime.Add(relativeValueChanged);
		queryCache->WasPressed.Add(wasPressed);
	}
}


void AAI2PlayerController::RecordInputData(FKey keyName, bool isAxis, float relativeValueChanged, float axisValue) {
	FPawnInputData* queryCache = FindKeyRecord("keyName");
	//if key never been pressed yet, create a new record
	if (queryCache == nullptr) {
		FPawnInputData keyEntry;
		keyEntry.KeyName = keyName;
		keyEntry.IsAxisKey = isAxis;
		keyEntry.ValueChangeOnTime.Add(relativeValueChanged);
		keyEntry.AxisValue.Add(axisValue);
		CloneDataInventory[ActiveCloneIndex]->InputData.Add(keyEntry);
	}
	//if key has been recorded, just add data to key record
	else {
		queryCache->ValueChangeOnTime.Add(relativeValueChanged);
		queryCache->AxisValue.Add(axisValue);
	}
}

void AAI2PlayerController::OnMoveForward(float p_val) {
	//for recording mechanic
	if (CurrentControlState == EControlState::CS_Enactor) {
		if (p_val > 0)
			RecordInputData(EKeys::W, timeElapsedAfterRecord, true);
		if (p_val < 0)
			RecordInputData(EKeys::S, timeElapsedAfterRecord, true);
	}
	if (GetControlledCharacter() && !GetControlledCharacter()->GetMovementComponent()->IsFalling()) {
		if (p_val > 0) {
			GetControlledCharacter()->InterpretKey(EKeys::W);
			AnyKeyPressedEvent.Broadcast();
		}
		else if (p_val < 0) {
			GetControlledCharacter()->InterpretKey(EKeys::S);
			AnyKeyPressedEvent.Broadcast();
		}
	}
}

void AAI2PlayerController::OnMoveRight(float p_val) {
	if (CurrentControlState == EControlState::CS_Enactor) {
		if (p_val > 0)
			RecordInputData(EKeys::D, timeElapsedAfterRecord, true);
		if (p_val < 0)
			RecordInputData(EKeys::A, timeElapsedAfterRecord, true);
	}
	if (GetControlledCharacter() && !GetControlledCharacter()->GetMovementComponent()->IsFalling()) {
		if (p_val > 0) {
			GetControlledCharacter()->InterpretKey(EKeys::D);
			AnyKeyPressedEvent.Broadcast();
		}
		else if (p_val < 0) {
			GetControlledCharacter()->InterpretKey(EKeys::A);
			AnyKeyPressedEvent.Broadcast();
		}
	}
}

void AAI2PlayerController::OnTurn(float p_val) {
	if (CurrentControlState == EControlState::CS_Enactor) {
		if (p_val < 1 && p_val != 0)
			RecordInputData(EKeys::LeftBracket, timeElapsedAfterRecord, true);
		else if (p_val > -1 && p_val != 0)
			RecordInputData(EKeys::RightBracket,timeElapsedAfterRecord,true);
	}
	if (GetPawn()) {
		if (GetControlledCharacter()->isAiming) {
			if(p_val < 1 && p_val != 0)
				GetControlledCharacter()->InterpretKey(EKeys::LeftBracket);
			else if(p_val > -1 && p_val != 0)
				GetControlledCharacter()->InterpretKey(EKeys::RightBracket);
		}
		if (!GetControlledCharacter()->CurrentTarget)
			GetPawn()->AddControllerYawInput(p_val);
	}
}

void AAI2PlayerController::OnLookUp(float p_val) {
	if (CurrentControlState == EControlState::CS_Enactor) {
		if (p_val > 1 && p_val != 0)
			RecordInputData(EKeys::PageUp, timeElapsedAfterRecord, true);
		else if (p_val < -1 && p_val != 0)
			RecordInputData(EKeys::PageDown, timeElapsedAfterRecord, true);
	}
	if (GetPawn()) {
		if (GetControlledCharacter()->isAiming) {
			if (p_val > 1 && p_val != 0)
				GetControlledCharacter()->InterpretKey(EKeys::PageUp);
			else if (p_val < -1 && p_val != 0)
				GetControlledCharacter()->InterpretKey(EKeys::PageDown);
		}
		if(!GetControlledCharacter()->CurrentTarget)
			GetPawn()->AddControllerPitchInput(p_val);
	}
}

void AAI2PlayerController::OnRecordPressed() {
	RecordHoldTime = 0.0f;
	bIsRecordPressed = true;
}

void AAI2PlayerController::OnRecordReleased() {
		if (RecordHoldTime <= RECORDHOLDTIMEREQUIRED && !bIsRecordingDisabled)
			ToggleRecord();
		bIsRecordPressed = false;
}

void AAI2PlayerController::OnJumpPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::SpaceBar, timeElapsedAfterRecord, true);
	if (GetControlledCharacter())
		GetControlledCharacter()->InterpretKey(EKeys::SpaceBar);
}

void AAI2PlayerController::OnSprintPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::LeftShift, timeElapsedAfterRecord, true);
	if (GetControlledCharacter())
		GetControlledCharacter()->InterpretKey(EKeys::LeftShift);
}

void AAI2PlayerController::OnCrouchPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::C, timeElapsedAfterRecord, true);
	if (GetControlledCharacter()) {
		GetControlledCharacter()->InterpretKey(EKeys::C);
		AnyKeyPressedEvent.Broadcast();
	}
}

void AAI2PlayerController::OnFirePressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::LeftMouseButton, timeElapsedAfterRecord, true);
	if (GetControlledCharacter())
		if (GetControlledCharacter()->GetCurrentState()->bCanUseWeapon) {
			GetControlledCharacter()->Fire();
			AnyKeyPressedEvent.Broadcast();
		}
}

void AAI2PlayerController::OnFireReleased() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::LeftMouseButton, timeElapsedAfterRecord, false);
	if (GetControlledCharacter())
		if (GetControlledCharacter()->GetCurrentState()->bCanUseWeapon)
			GetControlledCharacter()->StopFiring();
}

void AAI2PlayerController::OnAimPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::RightMouseButton, timeElapsedAfterRecord, true);
	if (GetControlledCharacter()) {
		if (GetControlledCharacter()->GetCurrentState()->bCanUseWeapon) {
			GetControlledCharacter()->Aim();
			AnyKeyPressedEvent.Broadcast();
		}
	}
}

void AAI2PlayerController::OnAimReleased() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::RightMouseButton, timeElapsedAfterRecord, false);
	if (GetControlledCharacter()) {
		if (GetControlledCharacter()->GetCurrentState()->bCanUseWeapon)
			GetControlledCharacter()->StopAiming();
	}
}

void AAI2PlayerController::OnReloadPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::R, timeElapsedAfterRecord, true);
	if (GetControlledCharacter()) {
		GetControlledCharacter()->EnterReload();
		AnyKeyPressedEvent.Broadcast();
	}
}

void AAI2PlayerController::OnCoverPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::C, timeElapsedAfterRecord, true);
	if (GetControlledCharacter()) {
		GetControlledCharacter()->InterpretKey(EKeys::C);
		AnyKeyPressedEvent.Broadcast();
	}
}

void AAI2PlayerController::OnInteractPressed() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::F, timeElapsedAfterRecord, true);
	if (GetControlledCharacter()) {
		GetControlledCharacter()->InterpretKey(EKeys::F);
		AnyKeyPressedEvent.Broadcast();
	}
}

void AAI2PlayerController::OnInteractReleased() {
	if (CurrentControlState == EControlState::CS_Enactor)
		RecordInputData(EKeys::F, timeElapsedAfterRecord, false);
	if (GetControlledCharacter())
		GetControlledCharacter()->StopInteract();
}

void AAI2PlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	//During recording, record actions of enactor(which is current pawn)
	if (CurrentControlState == EControlState::CS_Enactor) {
		//record transform and control rotation data
		CloneDataInventory[ActiveCloneIndex]->TransformData.Add(GetPawn()->GetActorTransform());
		CloneDataInventory[ActiveCloneIndex]->ControlRotData.Add(GetControlRotation());
		RecordFrequencyIndex += DeltaTime;
		//decrease current recordtime pool
		CurRecordTimePool -= DeltaTime;
		if (RecordFrequencyIndex <= RecordFrequency) {
			timeElapsedAfterRecord += DeltaTime;
			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Blue, (TEXT("%s"), FString::SanitizeFloat(timeElapsedAfterRecord)));
			RecordFrequencyIndex = 0.0f;
		}
		//if reached max record time or if record is disabled for some reason
		if (timeElapsedAfterRecord >= MAXRECORDTIME ||
			CurRecordTimePool < 0) { //if does not have enough resource to record
			//disable inputs
			DisableInput(this);
			//back to normal phase
			CurrentControlState = EControlState::CS_Rewind;
			//Fire gameplay events
			CloningStartedEvent.Broadcast();
			RecordEndedEvent.Broadcast();
		}
	}
		/*

		//Hide our enactor
		RecordEnactorCharacter->SetActorHiddenInGame(true);
		RecordEnactorCharacter->SetActorEnableCollision(false);
		//back to initial character
		Possess(InitialCharacter);
		//enable input
		EnableInput(this);
		//show clone in game
		CloneDataInventory[ActiveCloneIndex]->SetCloneActive(true);
		//Copy our player data
		float CharacterHP = InitialCharacter->CharacterStatsComponent->GetHealth();
		CloneDataInventory[ActiveCloneIndex]->Character->CharacterStatsComponent->SetHealth(CharacterHP);
		bool DoesPlayerHaveWeapon = (InitialCharacter->currentEquippedWeapon &&
			CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon) ? true : false;
		if (DoesPlayerHaveWeapon) {
			int CharacterClipSize = InitialCharacter->currentEquippedWeapon->CurrentClipSize;
			CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon->CurrentClipSize = CharacterClipSize;
			int CharacterMaxClip = InitialCharacter->currentEquippedWeapon->MaxClipSize;
			CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon->MaxClipSize = CharacterMaxClip;
		}
		CloneDataInventory[ActiveCloneIndex]->Character->isAiming = InitialCharacter->isAiming;
		CloneDataInventory[ActiveCloneIndex]->Character->isFiring = InitialCharacter->isFiring;
		CloneDataInventory[ActiveCloneIndex]->Character->isInteracting = InitialCharacter->isInteracting;
		//mark our clones to start replaying
		CloneDataInventory[ActiveCloneIndex]->bIsReplaying = true;
		//move active clone index
		ActiveCloneIndex = (ActiveCloneIndex < (MAXSPAWNABLECLONES - 1)) ? ActiveCloneIndex + 1 : 0;
		//change state
		CurrentControlState = EControlState::CS_Normal;
	}
	*/
	//}

	numberOfReplayingClones = 0;

	/*Call replay in Tick*/
	//foreach clone in inventory
	for (int i = 0; i < MAXSPAWNABLECLONES; i++) {
		//if current index is replaying
		if (CloneDataInventory[i]->bIsReplaying) {
			//add to integer counter
			numberOfReplayingClones++;
			//replay positioning
			if (CloneDataInventory[i]->TransformIndex < CloneDataInventory[i]->TransformData.Num() - 1) {
				//replay inputs
				TestReplay(CloneDataInventory[i], DeltaTime);
				//replay transform & control rotation
				CloneDataInventory[i]->Character->SetActorTransform(CloneDataInventory[i]->TransformData[CloneDataInventory[i]->TransformIndex]);
				CloneDataInventory[i]->TransformIndex++;
				Cast<APlayerController>(CloneDataInventory[i]->Character->Controller)->SetControlRotation(CloneDataInventory[i]->ControlRotData[CloneDataInventory[i]->CtrlRotIndex]);
				CloneDataInventory[i]->CtrlRotIndex++;
			}
			else {
				//hide the clone when replay over.
				CloneDataInventory[i]->ClearData();
				CloneDataInventory[i]->SetCloneActive(false);
				CloneDataInventory[i]->Character->PrimitiveIndicatorDebug->SetVisibility(false);
				CloneDataInventory[i]->bIsReplaying = false;
				CloneDataInventory[i]->Character->StopInteract();
				GetInitialCharacter()->UnlockMovements();
				CloningEndedEvent.Broadcast();
			}
		}
	}

	//Updating replay time in Tick
	if(CurrentControlState == EControlState::CS_Normal) { 
		if (numberOfReplayingClones > 0) { //increment only once at end of loop
			replayTimeElapsed += DeltaTime;
		}
		//regenerate record time pool to max when no clones active
		if (numberOfReplayingClones == 0)
			if (CurRecordTimePool < MAX_RECORDTIMEPOOL)
				CurRecordTimePool += DeltaTime * 1.2;
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Orange, (TEXT("%s"), FString::SanitizeFloat(CurRecordTimePool)));
}

void AAI2PlayerController::ClearRecordedData() {
	for (int i = 0; i < MAXSPAWNABLECLONES; i++) {
		CloneDataInventory[i]->bIsReplaying = false;
		CloneDataInventory[i]->InputData.Empty();
		CloneDataInventory[i]->TransformData.Empty();
		CloneDataInventory[i]->ControlRotData.Empty();
		CloneDataInventory[i]->TransformIndex = 0;
		CloneDataInventory[i]->CtrlRotIndex = 0;
	}
}

void AAI2PlayerController::ToggleRecord() {
	switch (CurrentControlState) {
	case EControlState::CS_Normal: {
		if (CurRecordTimePool > 0) {//does player have enough resources to record?
		//Empty Recorded Data first
			timeElapsedAfterRecord = 0.0f;
			replayTimeElapsed = 0;
			//if clone instantiated properly
			if (CloneDataInventory[ActiveCloneIndex]) {
				//reset enactor in our current location
				RecordEnactorCharacter->SetActorTransform(GetPawn()->GetActorTransform());
				//show our enactor
				RecordEnactorCharacter->SetActorHiddenInGame(false);
				RecordEnactorCharacter->SetActorEnableCollision(true);
				//possess the clone
				Possess(RecordEnactorCharacter);
				//copy initial pawn values to enactor
				float CharacterHP = InitialCharacter->CharacterStatsComponent->GetHealth();
				RecordEnactorCharacter->CharacterStatsComponent->SetHealth(CharacterHP);
				bool DoesPlayerHaveWeapon = (InitialCharacter->currentEquippedWeapon &&
					RecordEnactorCharacter->currentEquippedWeapon) ? true : false;
				if (DoesPlayerHaveWeapon) {
					int CharacterClipSize = InitialCharacter->currentEquippedWeapon->CurrentClipSize;
					RecordEnactorCharacter->currentEquippedWeapon->CurrentClipSize = CharacterClipSize;
					int CharacterMaxClip = InitialCharacter->currentEquippedWeapon->MaxClipSize;
					RecordEnactorCharacter->currentEquippedWeapon->MaxClipSize = CharacterMaxClip;
					int CharacterMaxAmmo = InitialCharacter->currentEquippedWeapon->TotalAmmo;
					RecordEnactorCharacter->currentEquippedWeapon->TotalAmmo = CharacterMaxAmmo;
				}
				RecordEnactorCharacter->isAiming = InitialCharacter->isAiming;
				RecordEnactorCharacter->isFiring = InitialCharacter->isFiring;
				RecordEnactorCharacter->isInteracting = InitialCharacter->isInteracting;
			}
			//we are now in record-enactor phase
			CurrentControlState = EControlState::CS_Enactor;
			//fire started record event
			RecordStartedEvent.Broadcast();
		}
	}
		break;

	case EControlState::CS_Enactor:
		//disable inputs
		DisableInput(this);
		//back to normal phase
		CurrentControlState = EControlState::CS_Rewind;
		//Fire gameplay events
		CloningStartedEvent.Broadcast();
		RecordEndedEvent.Broadcast();
		break;

	case EControlState::CS_Rewind:
		//kekers
		break;
	}
}

void AAI2PlayerController::SetRewindBackToNormal() {
	//Hide our enactor
	RecordEnactorCharacter->SetActorHiddenInGame(true);
	RecordEnactorCharacter->SetActorEnableCollision(false);
	//show clone in game
	CloneDataInventory[ActiveCloneIndex]->SetCloneActive(true);
	//Copy our player data
	float CharacterHP = InitialCharacter->CharacterStatsComponent->GetHealth();
	CloneDataInventory[ActiveCloneIndex]->Character->CharacterStatsComponent->SetHealth(CharacterHP);
	bool DoesPlayerHaveWeapon = (InitialCharacter->currentEquippedWeapon &&
		CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon) ? true : false;
	if (DoesPlayerHaveWeapon) {
		int CharacterClipSize = InitialCharacter->currentEquippedWeapon->CurrentClipSize;
		CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon->CurrentClipSize = CharacterClipSize;
		int CharacterMaxClip = InitialCharacter->currentEquippedWeapon->MaxClipSize;
		CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon->MaxClipSize = CharacterMaxClip;
		int CharacterMaxAmmo = InitialCharacter->currentEquippedWeapon->TotalAmmo;
		CloneDataInventory[ActiveCloneIndex]->Character->currentEquippedWeapon->TotalAmmo = CharacterMaxAmmo;
	}
	CloneDataInventory[ActiveCloneIndex]->Character->isAiming = InitialCharacter->isAiming;
	CloneDataInventory[ActiveCloneIndex]->Character->isFiring = InitialCharacter->isFiring;
	CloneDataInventory[ActiveCloneIndex]->Character->isInteracting = InitialCharacter->isInteracting;
	//mark our clones to start replaying
	CloneDataInventory[ActiveCloneIndex]->bIsReplaying = true;
	//move active clone index
	ActiveCloneIndex = (ActiveCloneIndex < (MAXSPAWNABLECLONES - 1)) ? ActiveCloneIndex + 1 : 0;
	//change state
	CurrentControlState = EControlState::CS_Normal;
	//back to initial character
	Possess(InitialCharacter);
	//enable input
	EnableInput(this);
}

void AAI2PlayerController::ActivateCloneReplay() {
	for (int i = 0; i < MAXSPAWNABLECLONES; i++) {
		CloneDataInventory[i]->bIsReplaying = true;
	}
}

void AAI2PlayerController::DeactivateCloneReplay() {
	for (int i = 0; i < MAXSPAWNABLECLONES; i++) {
		CloneDataInventory[i]->bIsReplaying = false;
	}
	GetInitialCharacter()->UnlockMovements();
}

void AAI2PlayerController::TestReplay(FCloneSlotData* target, float deltaTime) {
	//test translator logic
	for (int i = 0; i < target->InputData.Num(); i++) {
		for (int j = 0; j < target->InputData[i].ValueChangeOnTime.Num(); j++) {
			float toleranceVal = 0.01f;
			if ((target->InputData[i].ValueChangeOnTime[j] >= (replayTimeElapsed - toleranceVal))
				&& (target->InputData[i].ValueChangeOnTime[j] <= (replayTimeElapsed + toleranceVal))) {  //if matches deltaTime within a tolerance value
				if ((target->InputData[i].KeyName == EKeys::W)) {
					target->Character->InterpretKey(EKeys::W);
				}
				if ((target->InputData[i].KeyName == EKeys::S)) {
					target->Character->InterpretKey(EKeys::S);
				}
				if ((target->InputData[i].KeyName == EKeys::A)) {
					target->Character->InterpretKey(EKeys::A);
				}
				if ((target->InputData[i].KeyName == EKeys::D)) {
					target->Character->InterpretKey(EKeys::D);
				}

				//camera
				//doesnt work :c
				if ((target->InputData[i].KeyName == EKeys::MouseX)) {
					if (target->InputData[i].AxisValue[j] != 0) {
						target->Character->AddControllerYawInput(target->InputData[i].AxisValue[j]);
					}
				}
				if ((target->InputData[i].KeyName == EKeys::MouseY)) {
					if (target->InputData[i].AxisValue[j] != 0) {
						target->Character->AddControllerPitchInput(target->InputData[i].AxisValue[j]);
					}
				}

				if ((target->InputData[i].KeyName == EKeys::LeftShift) && (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::LeftShift);
				}
				if ((target->InputData[i].KeyName == EKeys::SpaceBar) && (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::SpaceBar);
				}
				if ((target->InputData[i].KeyName == EKeys::RightMouseButton) && (target->InputData[i].WasPressed[j])) {
					target->Character->Aim();
				}
				if ((target->InputData[i].KeyName == EKeys::RightMouseButton) && !(target->InputData[i].WasPressed[j])) {
					target->Character->StopAiming();
				}
				if ((target->InputData[i].KeyName == EKeys::R) && (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::R);
				}
				if ((target->InputData[i].KeyName == EKeys::LeftMouseButton) && (target->InputData[i].WasPressed[j])) {
					target->Character->Fire();
				}
				if ((target->InputData[i].KeyName == EKeys::LeftMouseButton) && !(target->InputData[i].WasPressed[j])) {
					target->Character->StopFiring();
				}
				if ((target->InputData[i].KeyName == EKeys::C) && (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::C);
				}
				if ((target->InputData[i].KeyName == EKeys::F) && (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::F);
				}
				if ((target->InputData[i].KeyName == EKeys::F) && !(target->InputData[i].WasPressed[j])) {
					target->Character->StopInteract();
				}
				//aiming boi
				if ((target->InputData[i].KeyName == EKeys::LeftBracket) 
					&& (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::LeftBracket);
				}
				if ((target->InputData[i].KeyName == EKeys::RightBracket) 
					&& (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::RightBracket);
				}
				if ((target->InputData[i].KeyName == EKeys::PageUp)
					&& (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::PageUp);
				}
				if ((target->InputData[i].KeyName == EKeys::PageDown)
					&& (target->InputData[i].WasPressed[j])) {
					target->Character->InterpretKey(EKeys::PageDown);
				}
			}
		}
	}
}