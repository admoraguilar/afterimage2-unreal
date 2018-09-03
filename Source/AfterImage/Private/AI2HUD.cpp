// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2HUD.h"

AAI2HUD::AAI2HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	CrosshairOffset = FVector2D(0.f, 0.f);
}

void AAI2HUD::DrawHUD() {
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	if (IsCrosshairVisible)
	{
		float ScaledSize = (CrosshairSize * Canvas->ClipY) / 1080.f;
		const FVector2D CrosshairDrawPosition((Center.X - (ScaledSize * 0.5)), (Center.Y - (ScaledSize * 0.5f)));

		if (CrosshairMaterial)
		{
			DrawMaterialSimple(CrosshairMaterial, CrosshairDrawPosition.X, CrosshairDrawPosition.Y, ScaledSize, ScaledSize);
		}
	}
}