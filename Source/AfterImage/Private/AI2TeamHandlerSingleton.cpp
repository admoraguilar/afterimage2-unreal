// Copyright 2014-2017 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2TeamHandlerSingleton.h"

AAI2TeamHandlerSingleton::AAI2TeamHandlerSingleton(const FObjectInitializer& ObjectInitializer) {

}

void AAI2TeamHandlerSingleton::AddCharacterToTeam(ACharacter* character, ECharacterTeam team) {
	switch (team) {
		case ECharacterTeam::CT_Player:
			PlayerTeam.Add(character);
			break;
		case ECharacterTeam::CT_Enemy:
			EnemyTeam.Add(character);
			break;
		case ECharacterTeam::CT_Neutral:
			NeutralTeam.Add(character);
			break;
		case ECharacterTeam::CT_Free:
			FreeTeam.Add(character);
			break;
	}
}

bool AAI2TeamHandlerSingleton::CheckCharacterIsOfTeam(ACharacter* character, ECharacterTeam team) {
	switch (team) {
		case ECharacterTeam::CT_Player: {
			for (int i = 0; i < PlayerTeam.Num() - 1; i++) {
				if (PlayerTeam[i] == character)
					return true;
			}
			break;
		}

		case ECharacterTeam::CT_Enemy: {
			for (int i = 0; i < EnemyTeam.Num() - 1; i++) {
				if (EnemyTeam[i] == character)
					return true;
			}
			break;
		}

		case ECharacterTeam::CT_Neutral: {
			for (int i = 0; i < NeutralTeam.Num() - 1; i++) {
				if (NeutralTeam[i] == character)
					return true;
			}
			break;
		}

		case ECharacterTeam::CT_Free: {
			for (int i = 0; i < FreeTeam.Num() - 1; i++) {
				if (FreeTeam[i] == character)
					return true;
			}
			break;
		}
	}
	return false;
}

void AAI2TeamHandlerSingleton::RemoveCharacterFromTeam(ACharacter* character, ECharacterTeam team) {
	switch (team) {
		case ECharacterTeam::CT_Player:
			PlayerTeam.Remove(character);
			break;
		case ECharacterTeam::CT_Enemy:
			EnemyTeam.Remove(character);
			break;
		case ECharacterTeam::CT_Neutral:
			NeutralTeam.Remove(character);
			break;
		case ECharacterTeam::CT_Free:
			FreeTeam.Remove(character);
			break;
	}
}

void AAI2TeamHandlerSingleton::ClearAllTeams() {
	PlayerTeam.Empty();
	EnemyTeam.Empty();
	NeutralTeam.Empty();
	FreeTeam.Empty();
}