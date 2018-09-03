// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "AI2Character.h"
#include "AI2PlayerCamera.generated.h"

UCLASS()
class AFTERIMAGE_API AAI2PlayerCamera : public APlayerCameraManager
{
	GENERATED_BODY()
protected:
	float NormalFOV = 90.0f;
	float AimingFOV = 65.0f;
	float CrouchingFOV = 70.0f;
	float CoverFOV = 110.0f;
	FVector DefaultOffset;
	FVector CameraOffset;
	FTransform Destination;
	virtual void BeginPlay() override;
	virtual void UpdateCamera(float DeltaTime) override;
	void UpdateCameraFOV(float DeltaTime);
	void UpdateCameraOffset();
	void LerpCameraOffset(FVector target);
	AAI2Character* GetCustomPlayerPawn();
public:
	AAI2PlayerCamera();
	void SetCameraOffset(float x, float y, float z);
};
