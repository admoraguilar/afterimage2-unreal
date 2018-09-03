// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2Character.h"
#include "AI2PlayerCamera.h"
#include "Kismet/KismetMathLibrary.h"

AAI2PlayerCamera::AAI2PlayerCamera() {
	
}

void AAI2PlayerCamera::BeginPlay() {
	AAI2Character* pawn = PCOwner ? (AAI2Character*)PCOwner->GetPawn() : nullptr;
	DefaultOffset = pawn->GetCameraComponent()->GetRelativeTransform().GetLocation();
}

AAI2Character* AAI2PlayerCamera::GetCustomPlayerPawn() {
	return (AAI2Character*)PCOwner->GetPawn();
}

void AAI2PlayerCamera::UpdateCamera(float deltaTime) {
	UpdateCameraFOV(deltaTime);
	//CameraOffset = FVector(0, 0, 0);
	if (GetCustomPlayerPawn() != nullptr) {
		//if pawn is under cover
		if (UCoverState* coverCast = dynamic_cast<UCoverState*>(GetCustomPlayerPawn()->GetCurrentState())) {
			//CameraOffset = FVector(-10, 0 ,0);
			//GetCustomPlayerPawn()->GetCameraComponent()->SetRelativeLocation((GetCameraLocation() + FVector(-10, 0, 0)));
		}
	}
	/*
	FVector lerpVector = FMath::VInterpTo(GetCustomPlayerPawn()->GetCameraComponent()->GetRelativeTransform().GetLocation(),
		GetCustomPlayerPawn()->GetCameraComponent()->GetRelativeTransform().GetLocation() + CameraOffset, deltaTime, 500.0f);
	GetCustomPlayerPawn()->GetCameraComponent()->SetRelativeLocation(lerpVector);
	*/
	
	Super::UpdateCamera(deltaTime);
}

void AAI2PlayerCamera::LerpCameraOffset(FVector target) {
	FVector tempLocation = FMath::Lerp(GetCameraLocation(),	target, 0.1f);
	SetActorLocation(tempLocation);
}

void AAI2PlayerCamera::UpdateCameraFOV(float DeltaTime) {
	if (GetCustomPlayerPawn() != nullptr) {
		//add more FOV movement here
		//if targetting, lerp lower FOV
		if (GetCustomPlayerPawn()->isAiming) {
			//const float TargetFOV = pawn->isAiming ? AimingFOV : NormalFOV;
			DefaultFOV = FMath::FInterpTo(DefaultFOV, AimingFOV, DeltaTime, 20.0f);
			SetFOV(DefaultFOV);
			return;
		}

		//restart camera to default
		DefaultFOV = FMath::FInterpTo(DefaultFOV, NormalFOV, DeltaTime, 20.0f);
		SetFOV(DefaultFOV);
		return;
	}
	//add more camera movement here
}

void AAI2PlayerCamera::SetCameraOffset(float x, float y, float z) {
	CameraOffset = FVector(x, y, z);
 }