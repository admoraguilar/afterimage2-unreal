// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/Actor.h"
#include "AI2StateRestorationHandler.generated.h"

USTRUCT()
struct FActorRestoreData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
		AActor* Actor;
	UPROPERTY()
		FTransform Transform;
	UPROPERTY()
		bool bIsDead;
	FActorRestoreData() {}
};

UCLASS()
class AFTERIMAGE_API AAI2StateRestorationHandler : public AActor {
	GENERATED_BODY()
	bool bWasInitialized = false;
	TArray<FActorRestoreData*> ActorsToRestoreData;
public:	
	// Sets default values for this actor's properties
	AAI2StateRestorationHandler();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
		void BindEvents();
	UFUNCTION()
		void SaveActorsData();
	UFUNCTION()
		void RestoreActorsData();
};
