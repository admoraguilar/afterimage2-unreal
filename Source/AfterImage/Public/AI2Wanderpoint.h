// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "AI2Wanderpoint.generated.h"

UCLASS()
class AFTERIMAGE_API AAI2Wanderpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI2Wanderpoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AITools")
		TArray<ATargetPoint*> Wanderpoints;
};
