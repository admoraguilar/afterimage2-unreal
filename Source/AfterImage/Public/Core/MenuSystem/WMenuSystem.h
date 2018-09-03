// Copyright 2014-2017 Frolic Games, All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Core/StateMachine/WStateMachine.h"
#include "Core/Object/WActor.h"
#include "WMenuSystem.generated.h"




UENUM(BlueprintType)
enum class EMenuSystemName : uint8 {
	Default,
	Menu_Main,
};




UCLASS(Blueprintable)
class AFTERIMAGE_API UWMenuState : public UWObjectState {
	GENERATED_BODY()
};




UCLASS()
class AFTERIMAGE_API AWMenuSystem : public AWActor {
	GENERATED_BODY()

	AWMenuSystem();
	
	static TArray<class AWMenuSystem*> Instances;

	virtual void PreBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category="UWMenuState|Variables") UWStateMachine* MenuState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UWMenuState|Variables") EMenuSystemName MenuSystemName;

public:	
	UFUNCTION(BlueprintCallable, Category="WMenuManager|Methods") static AWMenuSystem* GetMenuSystem(EMenuSystemName Name);

	UFUNCTION(BlueprintCallable, Category="WMenuManager|Methods") void SetMenu(TSubclassOf<class UWMenuState> InState);
	UFUNCTION(BlueprintCallable, Category="WMenuManager|Methods") bool IsMenu(TSubclassOf<class UWMenuState> InState) const;
	UFUNCTION(BlueprintCallable, Category="WMenuManager|Methods") void PushMenu(TSubclassOf<class UWMenuState> InState);
	UFUNCTION(BlueprintCallable, Category="WMenuManager|Methods") void PopMenu();
};
