// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once
#include "Object.h"
#include "AI2CustomUtilityLibrary.generated.h"

UCLASS()
class AFTERIMAGE_API UAI2CustomUtilityLibrary : public UObject {
	GENERATED_BODY()
public:
	static FORCEINLINE bool Trace(
		UWorld* World,
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel = ECC_Pawn,
		bool ReturnPhysMat = false
		) {
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		//Ignore Actors
		TraceParams.AddIgnoredActor(ActorToIgnore);
		//Re-initialize hit info
		HitOut = FHitResult(ForceInit);
		//Trace!
		World->LineTraceSingleByChannel(
			HitOut,		//result
			Start,	//start
			End, //end
			CollisionChannel, //collision channel
			TraceParams
		);
		//Hit any Actor?
		return (HitOut.GetActor() != NULL);
	}
};