// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "WObject.generated.h"




/**
*
*/
UCLASS()
class AFTERIMAGE_API UWObject : public UObject {
	GENERATED_BODY()

public:
	virtual class UWorld* GetWorld() const;
};

