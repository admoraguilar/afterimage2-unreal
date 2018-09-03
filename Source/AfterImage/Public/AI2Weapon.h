// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "ParticleDefinitions.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h" 
#include "AI2Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	WS_Pistol	 		UMETA(DisplayName = "Pistol"),
	WS_Rifle			UMETA(DisplayName = "Rifle"),
	WS_Shotgun			UMETA(DisplayName = "Shotgun")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	WS_Stowed	 		UMETA(DisplayName = "Stowed"),
	WS_Ready	 		UMETA(DisplayName = "Ready"),
	WS_Aiming	 		UMETA(DisplayName = "Aimed Down Sight"),
	WS_SwitchIn 		UMETA(DisplayName = "Switch In"),
	WS_SwitchOut		UMETA(DisplayName = "Switch Out"),
	WS_Firing			UMETA(DisplayName = "Firing"),
	WS_Reload			UMETA(DisplayName = "Reloading")
};

USTRUCT()
struct FAI2WeaponShotInfo {
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
		FVector TraceStart;
	UPROPERTY()
		FVector TraceEnd;
	UPROPERTY()
		FVector HitLocation;
	UPROPERTY()
		FVector HitNormal;
	UPROPERTY()
		float Distance;
	UPROPERTY()
		AActor* HitActor;
	UPROPERTY()
		bool DidHit;
	UPROPERTY()
		bool IsCritical;
	FAI2WeaponShotInfo(){
		HitActor = NULL;
		IsCritical = false;
	}
	void Destroy() {
		HitActor = nullptr;
	}
};

UCLASS()
class AFTERIMAGE_API AAI2Weapon : public AActor {
	GENERATED_BODY()
	FVector StartTrace;
	FVector Target;
	float CriticalDamage;
public:	
	// Sets default values for this actor's properties
	AAI2Weapon(const class FObjectInitializer& PCIP);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundBase* FireSFX;
	UPROPERTY(EditDefaultsOnly, Category = Particle)
		UParticleSystem* MuzzleFireFX;
	UPROPERTY(EditDefaultsOnly, Category = Particle)
		UParticleSystem* HitFireFX;
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = Specs)
		bool IsAutomatic;
	UPROPERTY(EditDefaultsOnly, Category = Specs)
		bool IsFullReload = true;

	// Weapon Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		float Damage = 60.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		float CritMultiplier = 1.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		float FireRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		float ReloadSpeed;
	UPROPERTY(EditDefaultsOnly, Category = Stats)
		float MuzzleClimbDown = 16.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 CurrentClipSize = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 MaxClipSize = 30;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats)
		int32 TotalAmmo = 30;

	//Firing
	bool WantsToFire;
	bool NeedsReload();
	UPROPERTY(BlueprintReadWrite, Category="Gameplay")
		bool IsFiring;
	virtual bool CanFire();
	virtual void StartFire();
	void StopFire();
	virtual void ShootBullet();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void Reload();
	bool LastFire = false;
	void SetTargetAim(FVector PStart, FVector PTarget);
	void SpawnFX(FVector Location);
	void SetCriticalDamage(float CritDamage);
	bool CanReload();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void AddAmmo(int Amount);
};
