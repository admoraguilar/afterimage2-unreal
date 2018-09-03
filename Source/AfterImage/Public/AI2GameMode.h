// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/GameMode.h"
#include "AI2StateRestorationHandler.h"
#include "AI2TeamHandlerSingleton.h"
#include "AI2InteractionObserver.h"
#include "AI2GameMode.generated.h"

class AAI2InteractionObserver;
class AAI2StateRestorationHandler;
class AAI2TeamHandlerSingleton;
UCLASS()
class AFTERIMAGE_API AAI2GameMode : public AGameMode
{
	GENERATED_BODY()
	AAI2InteractionObserver* InteractionObserver;
	AAI2TeamHandlerSingleton* TeamHandler;
	AAI2StateRestorationHandler* StateRestorationHandler;

	UPROPERTY(EditDefaultsOnly, Category = "AAI2DefaultGameMode|Systems") TArray<TSubclassOf<AActor>> CustomSystems;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	AAI2GameMode(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category="GameMode")
		AAI2InteractionObserver* GetInteractionObserver() { return InteractionObserver; }
	UFUNCTION(BlueprintCallable, Category = "GameMode")
		AAI2TeamHandlerSingleton* GetTeamHandler();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayGlobal")
		class TSubclassOf<AActor> EnactorCharacterClass;
};
