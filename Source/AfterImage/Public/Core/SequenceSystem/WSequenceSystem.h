// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Core/Object/WActor.h"
#include "Core/StateMachine/WStateMachine.h"
#include "WSequenceSystem.generated.h"




UENUM(BlueprintType)
enum class ESequenceSystemName : uint8 {
	Default,
};




UCLASS(Blueprintable)
class AFTERIMAGE_API UWSequenceState : public UWObjectState {
	GENERATED_BODY()
};




UCLASS()
class AFTERIMAGE_API AWSequenceSystem : public AWActor {
	GENERATED_BODY()

		AWSequenceSystem();

	static TArray<class AWSequenceSystem*> Instances;

	virtual void PreBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "WSequenceSystem|Variables") UWStateMachine* SequenceState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WSequenceSystem|Variables") ESequenceSystemName SequenceSystemName;

public:
	UFUNCTION(BlueprintCallable, Category = "WSequenceSystem|Methods") static AWSequenceSystem* GetSequenceSystem(ESequenceSystemName Name);

	UFUNCTION(BlueprintCallable, Category = "WSequenceSystem|Methods") void SetSequence(TSubclassOf<class UWSequenceState> InState);
	UFUNCTION(BlueprintCallable, Category = "WSequenceSystem|Methods") bool IsSequence(TSubclassOf<class UWSequenceState> InState) const;
	UFUNCTION(BlueprintCallable, Category = "WSequenceSystem|Methods") void PushSequence(TSubclassOf<class UWSequenceState> InState);
	UFUNCTION(BlueprintCallable, Category = "WSequenceSystem|Methods") void PopSequence();
};
