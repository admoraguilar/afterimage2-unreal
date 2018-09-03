// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2Character.h"
#include "AI2Weapon.h"
#include "AI2Damageable.h"
//#include "EngineKismetLibraryClasses.h"

// Sets default values
AAI2Weapon::AAI2Weapon(const class FObjectInitializer& PCIP) {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Setup weapon mesh
	if(Mesh == NULL) //don't replace mesh with empty mesh on compile if a mesh is already assigned
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //don't collide with player
	CurrentClipSize = MaxClipSize; //replenish clip
}

// Called when the game starts or when spawned
void AAI2Weapon::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AAI2Weapon::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	if (WantsToFire) {
		if (CanFire()) {
			FTimerHandle UnusedHandle;
			if (LastFire == false) {
				LastFire = true;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &AAI2Weapon::ShootBullet, 0.1f, false);
			}
			//ShootBullet();
		}
		else
			StopFire();
	}
}

bool AAI2Weapon::NeedsReload() {
	return (CurrentClipSize > 0)?false:true;
}

bool AAI2Weapon::CanReload() {
	return (TotalAmmo > 0) ? true : false;
}

bool AAI2Weapon::CanFire() {
	if(NeedsReload())
		return false;
	return true;
}

void AAI2Weapon::SetTargetAim(FVector PStart, FVector PTarget) {
	StartTrace = PStart;
	Target = PTarget;
}

void AAI2Weapon::StartFire() {
	WantsToFire = true;
}

void AAI2Weapon::StopFire() {
	WantsToFire = false;
	IsFiring = false;
}

void AAI2Weapon::SetCriticalDamage(float CritDamage) {
	CriticalDamage = CritDamage;
}

void AAI2Weapon::ShootBullet() {
	//Trace from mesh to screen center
	const FName TraceTag("WeaponTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(Instigator);
	TraceParams.TraceTag = TraceTag;
	GetWorld()->DebugDrawTraceTag = TraceTag;

	if (Instigator) {
		GetWorld()->LineTraceSingleByChannel(
			HitData,				//result
			StartTrace,	//start
			Target,		//end
			ECC_GameTraceChannel2,	// ECC_EngineTraceChannel2 is for Bullets
			TraceParams
		);

		SpawnFX(HitData.Location);

		//decrease clip
		if (CurrentClipSize > 0) {
			CurrentClipSize--;
		}

		if (HitData.Actor != NULL) {
			if (HitData.Actor->GetClass()->ImplementsInterface(UAI2Damageable::StaticClass())) {
				IAI2Damageable::Execute_TakeDamage(HitData.Actor.Get(), 0, 0.1f, 0.5f, CriticalDamage, this, HitData);
			}
		}
	}
	LastFire = false;
}

void AAI2Weapon::Reload() {
	int AmmoToReload = MaxClipSize - CurrentClipSize;

	if (TotalAmmo > 0) {
		if ((TotalAmmo - AmmoToReload) > 0) {
			CurrentClipSize += AmmoToReload;
			TotalAmmo -= AmmoToReload;
		}
		else {
			CurrentClipSize += TotalAmmo;
			TotalAmmo = 0;
		}
	}
	//CurrentClipSize = MaxClipSize;
}

void AAI2Weapon::SpawnFX(FVector Location) {
	//play muzzle fire vfx
	UGameplayStatics::SpawnEmitterAttached(MuzzleFireFX,
		Mesh,
		FName("s_muzzle"),
		FVector(0, 0, 0),
		FRotator(0, 90, 0),
		EAttachLocation::KeepRelativeOffset,
		true);

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		HitFireFX,
		Location,
		FRotator(0, 0, 0),
		true);

	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		FireSFX,
		Location,
		1.f
	);
}

void AAI2Weapon::AddAmmo(int Amount) {
	TotalAmmo += Amount;
}