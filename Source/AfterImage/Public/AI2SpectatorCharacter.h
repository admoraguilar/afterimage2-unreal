// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2Character.h"
#include "AI2SpectatorCharacter.generated.h"

UCLASS()
class AFTERIMAGE_API AAI2SpectatorCharacter : public AAI2Character
{
	GENERATED_BODY()
	
	AAI2SpectatorCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
};

UCLASS(Blueprintable)
class AFTERIMAGE_API USpectator_Move : public UCharacterState {
	GENERATED_BODY()
public:
	USpectator_Move() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	//override base cover transitions
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character) {}
	virtual void OnExit(AAI2Character* character) {}
};