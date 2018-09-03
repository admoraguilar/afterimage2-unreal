// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "GameFramework/HUD.h"
#include "AI2HUD.generated.h"

/**
 * 
 */
UCLASS()
class AFTERIMAGE_API AAI2HUD : public AHUD
{
	GENERATED_BODY()
public:
	//constructor
	AAI2HUD(const FObjectInitializer& ObjectInitializer);
	virtual void DrawHUD() override;
	float CrosshairSize = 2000.f;
	bool IsCrosshairVisible = true;
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
		UMaterialInstance * CrosshairMaterial;
private:
	FVector2D CrosshairOffset;
	class UTexture2D* CrosshairTex;
};
