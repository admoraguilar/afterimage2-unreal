// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "Components/ActorComponent.h"
#include "AI2TargetComponent.generated.h"

USTRUCT()
struct FBodyPartData {
	GENERATED_USTRUCT_BODY()
public:
	FBodyPartData() {}
	FBodyPartData(FString name, FTransform transform) {
		BodyPartName = name;
		BodyPartTransform = transform;
	}
	UPROPERTY(BlueprintReadOnly, Category="BodyPart")
		FString BodyPartName;
	UPROPERTY(BlueprintReadOnly, Category = "BodyPart")
		FTransform BodyPartTransform;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AFTERIMAGE_API UAI2TargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAI2TargetComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category="Gameplay")
		TArray<FBodyPartData> TargettableBodyParts;
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void AddBodyPart(FString partName, FTransform partTransform);
};
