// Fill out your copyright notice in the Description page of Project Settings.

#include "AfterImage.h"
#include "WObject.h"




UWorld* UWObject::GetWorld() const {
	UObject* Outer = GetOuter();
	UWorld* World = nullptr;

	if (Outer->IsA(AActor::StaticClass())) {
		World = Outer->GetWorld();
	}

	return World;
}