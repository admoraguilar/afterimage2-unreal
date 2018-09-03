// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#pragma once

#include "AI2Weapon.h"
#include "AI2GameMode.h"
#include "AI2BaseNPC.h"
#include "InputCoreTypes.h"
#include "AI2StatsComponent.h"
#include "AI2PlayerController.h"
#include "AI2TargetComponent.h"
#include "GameFramework/Character.h"
#include "AI2Character.generated.h"

class AAI2BaseNPC;
class UCharacterState;
class UTargetComponent;

struct CoverData {
public:
	CoverData() {}
	FVector CoverLocation;
	FVector CoverNormal;
	FRotator CoverRotation;
};

USTRUCT()
struct FTarget {
	GENERATED_USTRUCT_BODY()
	FTarget() {}
	UPROPERTY()	
		FName SocketName;
	UPROPERTY()
		float VertVal;
	UPROPERTY()
		float HorVal;
	UPROPERTY()
		bool IsWeakSpot = false;
	FTarget(FName PSocketName, float PVertVal, float PHorVal, bool PIsWeakSpot) {
		SocketName = PSocketName;
		VertVal = PVertVal;
		HorVal = PHorVal;
		IsWeakSpot = PIsWeakSpot;
	}
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterAnimEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterAnimEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRequestInteractableEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAcquiredWeaponEvent, AAI2Character*, CharacterAcquirer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerCharacterEvent);

UCLASS()
class AFTERIMAGE_API AAI2Character : public ACharacter {
	GENERATED_BODY()
protected:
	bool CanSwitchTarget = true;
	FTarget CurTargetPart;
	FTarget DefaultTargetPart;
	TMap<FString, FTarget> TargetData;
	// Character Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;
	CoverData* CurrentCoverData = new CoverData();
	UCharacterState* CurrentState;
	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		bool isReloading;
	bool CanFire();
	bool CanAim();
	USkeletalMeshComponent* TargetSkeletonMesh;
public: 
	TArray<AAI2BaseNPC*> AimTargetables;
	AAI2BaseNPC* CurrentTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PrimitiveIndicatorDebug;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* AimCollisionMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* HipFireWeaponDestination;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
		class UAI2StatsComponent* CharacterStatsComponent;
	//Gameplay Events
	UPROPERTY(BlueprintAssignable, Category = "AnimationEvent")
		FCharacterAnimEvent OnStartAim;
	UPROPERTY(BlueprintAssignable, Category = "AnimationEvent")
		FCharacterAnimEvent OnExitAim;
	UPROPERTY(BlueprintAssignable, Category = "AnimationEvent")
		FCharacterAnimEvent OnStartVault;
	UPROPERTY(BlueprintAssignable, Category = "CombatEvent")
		FAcquiredWeaponEvent OnAcquireWeapon;
	UPROPERTY(BlueprintAssignable, Category = "CombatEvent")
		FCharacterAnimEvent OnSwitchedTargetPart;
	//Combat
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool bAllowPlayerInput;
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool isFiring;
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool isAiming;
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool isInteracting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		AAI2Weapon* RiflePresetWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		AAI2Weapon* currentEquippedWeapon;
	//Anim montage to play when reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ReloadMontage;
	UPROPERTY(BlueprintAssignable, Category = "GameplayDelegate")
		FRequestInteractableEvent OnRequestInteractableDelegate;
	//*Movement*//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float AimingWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float CrouchWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float SprintSpeed;
	UFUNCTION(BlueprintCallable, Category = "Camera")
		UCameraComponent* GetCameraComponent();
	UFUNCTION(BlueprintCallable,Category="CharacterState")
		UCharacterState* GetCurrentState() { return CurrentState; }
	UFUNCTION(BlueprintCallable, Category = AnimNotify)
		void ReloadWeapon();
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool CoverCheckFacingRight();
	CoverData* GetCurrentCoverData() { return CurrentCoverData; }
	bool coverMovingRight;
	//For Character States
	void InterpretKey(FKey input);
	template<class T>
	void SetCharacterState();
	void ToggleLockRotation(bool p_lock);
	void ComputeForWeaponTraceForward();
public:
	// Contructor
	AAI2Character(const FObjectInitializer& ObjectInitializer);
	//Initialization Functions
	void InitializeTargetData();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	//movement functions
	void MoveForward(float p_val);
	void MoveRight(float p_val);
	//camera functions
	void Turn(int dir);
	void LookUp(int dir);
	void Jump();
	void CoverMoveRight(float p_val);
	//auto move override
	float AutoMoveStoppingDistance= 110.0f;
	bool bAutoDestinationReached = false;
	UPROPERTY(BlueprintReadWrite, Category = "AI2Character")
		bool bIsMovementDisabled = false;
	FVector AutoMoveDestination;
	void SetAutoMoveDestination(FVector Destination);
	void RunAutoMove();
	//Sprint functions
	void EnterSprint();
	void ExitSprint();
	//crouch
	void EnterCrouch();
	void ExitCrouch();
	//combat
	void Fire();
	void StopFiring();
	//aiming
	void Aim();
	void StopAiming();
	//reload
	void EnterReload();
	//cover
	void EnterCover();
	void ExitCover();
	bool CanCoverMove();
	bool IsCoverHigh();
	//interaction
	void TryInteract();
	void StopInteract();
	void MakeMovementNoise();
	//vaulting
	FVector CheckForLedge();
	FHitResult TraceForCover();
	//weapon
	UFUNCTION(BlueprintCallable, Category="Combat")
		void EquipWeapon();
	bool HasWeapon();
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
		void LockMovements();
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
		void UnlockMovements();
	UFUNCTION()
		void AllowTargetPartSwitch(int dir);
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
		void OnCollisionMeshHitActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void SetCurrentTargetPart(FTarget BodyPart);
};

//*State Machine for Character--------------------------*///
UCLASS(Blueprintable)
class AFTERIMAGE_API UCharacterState : public UObject {
	GENERATED_BODY()
public:
	//virtual UCharacterState() {}
	virtual ~UCharacterState() {}
	virtual void HandleInput(AAI2Character* character, FKey input) {}
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character) {}
	virtual void OnExit(AAI2Character* character) {}
	bool bCanUseWeapon = true;
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UBaseState : public UCharacterState {
	GENERATED_BODY()
public:
	UBaseState() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UBase_IdleWalkState : public UBaseState {
	GENERATED_BODY()
public:
	UBase_IdleWalkState() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	//override base cover transitions
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character) {}
	virtual void OnExit(AAI2Character* character) {}
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UBase_SprintState : public UBaseState {
	GENERATED_BODY()
public:
	UBase_SprintState() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime);
	//override base cover transitions
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UBase_CrouchState : public UBaseState {
	GENERATED_BODY()
public:
	UBase_CrouchState() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UCoverState : public UCharacterState {
	GENERATED_BODY()
public:
	UCoverState() {
		bCanUseWeapon = false;
	}
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UCover_IdleWalkState : public UCoverState {
	GENERATED_BODY()
public:
	UCover_IdleWalkState() {
		bCanUseWeapon = false;
	}
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	//override base cover transitions
	virtual void OnEnter(AAI2Character* character) {}
	virtual void OnExit(AAI2Character* character) {}
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UCover_PeekState : public UCoverState {
	GENERATED_BODY()
public:
	UCover_PeekState() {
		bCanUseWeapon = true;
	}
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	//override base cover transitions
	virtual void OnEnter(AAI2Character* character) {}
	virtual void OnExit(AAI2Character* character) {}
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UCover_CrouchState : public UCover_IdleWalkState {
	GENERATED_BODY()
public:
	UCover_CrouchState() {
		bCanUseWeapon = false;
	}
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime) {}
	//override base cover transitions
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};


UCLASS(Blueprintable)
class AFTERIMAGE_API UAutoMoveState : public UCharacterState {
	GENERATED_BODY()
public:
	UAutoMoveState() {};
	virtual void HandleInput(AAI2Character* character, FKey input);
	virtual void Update(AAI2Character* character, float deltaTime);
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};

UCLASS(Blueprintable)
class AFTERIMAGE_API UVaultState : public UCharacterState {
	GENERATED_BODY()
public:
	UVaultState() {};
	virtual void HandleInput(AAI2Character* character, FKey input) {};
	virtual void Update(AAI2Character* character, float deltaTime);
	virtual void OnEnter(AAI2Character* character);
	virtual void OnExit(AAI2Character* character);
};