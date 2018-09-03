// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "WActor.generated.h"




UCLASS()
class AFTERIMAGE_API AWActor : public AActor {
	GENERATED_BODY()

protected:
	virtual void PostInitializeComponents() override;
	virtual void PreBeginPlay();

	UFUNCTION(BlueprintNativeEvent, Category = "WActor|Methods") void ExecPreBeginPlay();
	virtual void ExecPreBeginPlay_Implementation();
};
