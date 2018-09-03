// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/Actor.h"
#include "AI2TeamHandlerSingleton.generated.h"

UENUM(BlueprintType)
enum class ECharacterTeam : uint8 {
	CT_Player	 		UMETA(DisplayName = "Player"),
	CT_Enemy	 		UMETA(DisplayName = "Enemy"),
	CT_Neutral	 		UMETA(DisplayName = "Neutral"),
	CT_Free 			UMETA(DisplayName = "Free"),
	CT_NULL				UMETA(DisplayName = "wot de fuk")
};

UCLASS(Blueprintable, BlueprintType)
class AFTERIMAGE_API AAI2TeamHandlerSingleton : public AActor {
	GENERATED_BODY()
	TArray<ACharacter*> PlayerTeam;
	TArray<ACharacter*> EnemyTeam;
	TArray<ACharacter*> NeutralTeam;
	TArray<ACharacter*> FreeTeam;
public:
	AAI2TeamHandlerSingleton(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable, Category = "Team") 
		void AddCharacterToTeam(ACharacter* character, ECharacterTeam team);
	UFUNCTION(BlueprintCallable, Category = "Team")
		bool CheckCharacterIsOfTeam(ACharacter* character, ECharacterTeam team);
	UFUNCTION(BlueprintCallable, Category = "Team")
		void RemoveCharacterFromTeam(ACharacter* character, ECharacterTeam team);
	UFUNCTION(BlueprintCallable, Category = "Team")
		void ClearAllTeams();
};
