// Copyright 2014-2016 Frolic Games, All Rights Reserved.
// Author: Justine Orprecio, Lead Programmer

#include "AfterImage.h"
#include "AI2TeamHandlerSingleton.h"
#include "AI2InteractionObserver.h"
#include "AI2Character.h"
#include "AI2PlayerController.h"
#include "AI2CustomUtilityLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

class AAI2InteractionObserver;
// Sets default values
AAI2Character::AAI2Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//*CAMERA INIT*//
	PrimitiveIndicatorDebug = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugPrimitiveIndicator"));
	//create spring arm object
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	//attach spring arm to mesh
	CameraSpringArm->SetupAttachment(GetCapsuleComponent());
	//set camera spring arm variables
	CameraSpringArm->TargetArmLength = -85.0f;
	//position spring arm component
	CameraSpringArm->RelativeLocation = FVector(0.0f, 0.0f, 150.0f);
	CameraSpringArm->SocketOffset = FVector(-120.0f, 40.0f, 15.0f);
	//rotate spring arm using controller rotation
	CameraSpringArm->bUsePawnControlRotation = true;
	//create camera object
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//attach to root component capsule
	CameraComponent->SetupAttachment(CameraSpringArm);
	//Pawn emitter init
	PawnNoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawn Noise Emitter"));
	//Stats components init
	CharacterStatsComponent = CreateDefaultSubobject<UAI2StatsComponent>(TEXT("Character Stats"));

	//*MESH INIT*//
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	//*MOVEMENT INIT*//
	GetCharacterMovement()->bCrouchMaintainsBaseLocation = true;
	GetCameraComponent()->bUsePawnControlRotation = true;
}

template<class T>
void AAI2Character::SetCharacterState() {
	UCharacterState* tempHolder = NewObject<T>();//  new T();
	if (tempHolder) {
		//Because fuck garbage collector
		tempHolder->AddToRoot();
		//Call any exit state functionality
		CurrentState->OnExit(this);
		//Set new state and remove old state from root to let garbage collector do its job
		UCharacterState* t = CurrentState;
		CurrentState = tempHolder;
		t->RemoveFromRoot();
		//call current state on enter functionality
		CurrentState->OnEnter(this);
	}
}

UCameraComponent* AAI2Character::GetCameraComponent() {
	return CameraComponent;
}

void AAI2Character::InitializeTargetData() {
	//initialize targettable areas for combat
	//parameters: socket name, vertical value, horizontal value, is a weak spot?
	TargetData.Add("Head", FTarget("head", 3, 2, false));
	TargetData.Add("Body", FTarget("spine_03", 2, 2, false));
	TargetData.Add("LeftArm", FTarget("lowerarm_l", 2, 1, false));
	TargetData.Add("RightArm", FTarget("lowerarm_r", 2, 3, false));
	TargetData.Add("LeftLeg", FTarget("calf_l", 1, 1, false));
	TargetData.Add("RightLeg", FTarget("calf_r", 1, 3, false));
	TargetData.Add("Back", FTarget("spine_01", -1, -1, true));
	//set default value for current target
	//SetCurrentTargetPart(TargetData.FindRef("Body"));
	CurTargetPart = TargetData.FindRef("Body");
}

// Called when the game starts or when spawned
void AAI2Character::BeginPlay() {
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	//bind events
	if(AimCollisionMesh)
		AimCollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AAI2Character::OnCollisionMeshHitActor);
	//Initialize State
	CurrentState = NewObject<UBaseState>(); //new UBaseState();
	CurrentState->AddToRoot();
	AAI2GameMode* const gm = GetWorld()->GetAuthGameMode<AAI2GameMode>();
	if(gm->GetTeamHandler())
		gm->GetTeamHandler()->AddCharacterToTeam(this, ECharacterTeam::CT_Player);

	//hide primitive at start
	PrimitiveIndicatorDebug->SetVisibility(false, true);

	//Initialize target data parts
	InitializeTargetData();
}

void AAI2Character::OnCollisionMeshHitActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (isAiming && !CurrentTarget) {
		if (OtherActor->IsA<AAI2BaseNPC>()) {
			CurrentTarget = Cast<AAI2BaseNPC>(OtherActor);
			PrimitiveIndicatorDebug->SetVisibility(true, true);
		}
	}
}

// Called every frame
void AAI2Character::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	//auto reload
	if (currentEquippedWeapon != NULL)
		if(currentEquippedWeapon->NeedsReload() && currentEquippedWeapon->CanReload()){
			EnterReload();
	}

	//update current state if one exists
	if (GetCurrentState()) {
		GetCurrentState()->Update(this, DeltaTime);
		//lock rotation if shooting or in cover
		/*
		if (!Cast<UCoverState>(GetCurrentState())) {
			if (isAiming || isFiring) {
				bUseControllerRotationYaw = true;
			}
			else
				bUseControllerRotationYaw = false;
		}
		else
			bUseControllerRotationYaw = false;
			*/
	}

	//autolock aiming
	if (isAiming) {
		//Do I habe target
		if (CurrentTarget) {
			//Get Current Target actor's skeleton for socket references
			TargetSkeletonMesh = CurrentTarget->FindComponentByClass<USkeletalMeshComponent>();
			if (TargetSkeletonMesh) {
				//Compute for rotation between character and target body part
				FRotator actorLookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName));
				//Set target indicator location
				PrimitiveIndicatorDebug->SetWorldLocation(TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName));
				//Lerp Controller rotation to Look-At-Target rotation
				actorLookAtRot = FRotator(actorLookAtRot.Pitch, actorLookAtRot.Yaw - 5.f, actorLookAtRot.Roll);
				if (currentEquippedWeapon)
					currentEquippedWeapon->SetTargetAim(currentEquippedWeapon->GetActorLocation(), TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName));
				if (GetController()) {
					FRotator interpRot = UKismetMathLibrary::RInterpTo(GetController()->GetControlRotation(), actorLookAtRot, DeltaTime, 55.f);
					GetController()->SetControlRotation(interpRot);
				}
			}
		}

		//ReloadMontage != NULL && !isFullReload && !isReloading && currentEquippedWeapon->CanReload()
	}

	//update trace forward when hip firing
	if (isFiring && !CurrentTarget && currentEquippedWeapon) {
		ComputeForWeaponTraceForward();
	}

	if (CurrentTarget) {
		FRotator LookAtRotEnemy = UKismetMathLibrary::FindLookAtRotation(CurrentTarget->GetActorLocation(), GetActorLocation());
		if (CurTargetPart.SocketName == TargetData.FindRef("Back").SocketName) {
			if (LookAtRotEnemy.Equals(CurrentTarget->GetActorRotation(), 40.f)) {
				SetCurrentTargetPart(TargetData.FindRef("Body"));
			}
		}
		else
			if (!LookAtRotEnemy.Equals(CurrentTarget->GetActorRotation(), 40.f)) {
				SetCurrentTargetPart(TargetData.FindRef("Back"));
			}
	}
}

void AAI2Character::SetCurrentTargetPart(FTarget BodyPart) {
	CurTargetPart = BodyPart;
	if (currentEquippedWeapon) {
		if (CurTargetPart.IsWeakSpot) {
			currentEquippedWeapon->SetCriticalDamage(20);
		}
		else
			currentEquippedWeapon->SetCriticalDamage(0);
	}
}

void AAI2Character::SetAutoMoveDestination(FVector Destination) {
	EnterSprint();
	AutoMoveDestination = Destination;
	bAutoDestinationReached = false;
}

void AAI2Character::RunAutoMove() {
	MoveForward(1);
	float distance = FVector::Dist(GetActorLocation(), AutoMoveDestination);
	if (distance < AutoMoveStoppingDistance) {
		ExitSprint();
		bAutoDestinationReached = true;
	}
}

void AAI2Character::EnterSprint() {
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	if (!currentEquippedWeapon) {
		bUseControllerRotationYaw = false;
		ToggleLockRotation(false);
	}
}

void AAI2Character::ExitSprint() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (!currentEquippedWeapon) {
		bUseControllerRotationYaw = true;
		ToggleLockRotation(true);
	}
}

void AAI2Character::EnterCrouch() {
	GetCharacterMovement()->MaxWalkSpeed = CrouchWalkSpeed;
}

void AAI2Character::ExitCrouch() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAI2Character::MakeMovementNoise() {
	//make noise to be detected by AI
	/*
	if (Cast<UBase_CrouchState>(CurrentState)) {
		PawnNoiseEmitterComponent->MakeNoise(this, .1f, GetActorLocation());
	}
	else {
		if (Cast<UBase_SprintState>(CurrentState)) {
			PawnNoiseEmitterComponent->MakeNoise(this, 1.f, GetActorLocation());
		}
		else
			PawnNoiseEmitterComponent->MakeNoise(this, .5f, GetActorLocation());
	}
	*/
}

void AAI2Character::MoveForward(float p_val){
	if (p_val != 0) {
		FRotator Rotation = Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) {
			Rotation.Pitch = 0.0f;
		}
		//make noise to be detected by AI
		MakeMovementNoise();
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, p_val);
	}
}

void AAI2Character::MoveRight(float p_val) {
	if (p_val != 0.0f){
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		//make noise to be detected by AI
		MakeMovementNoise();
		// add movement in that direction
		AddMovementInput(Direction, p_val);
	}
}

void AAI2Character::LookUp(int dir) {
	if (CurrentTarget) {
		TargetSkeletonMesh = CurrentTarget->FindComponentByClass<USkeletalMeshComponent>();
		FVector targetLocation = TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName);
		FTimerDelegate TimerDel;
		FTimerHandle SwitchTargetTimer;
		TimerDel.BindUFunction(this, FName("AllowTargetPartSwitch"), dir);
		if (CanSwitchTarget == true) {
			//translate map to array because I need to access the map by index
			TArray<FTarget> tempTargetArray;
			TargetData.GenerateValueArray(tempTargetArray);
			for (int i = 0; i < tempTargetArray.Num(); i++) {
				//We're looking for "lower" body parts
				if (dir == -1) {
					//get the immediate target part with higher horizontal value than our current target part
					if (tempTargetArray[i].VertVal == (CurTargetPart.VertVal + 1)) {
						//set that as our current target
						SetCurrentTargetPart(tempTargetArray[i]);
						//aim weapon at indicator
						targetLocation= TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName);
						OnSwitchedTargetPart.Broadcast();
						//end loop, we done here fuckers
						break;
					}
				}
				//We're looking for "higher" body parts
				else if (dir == 1) {
					//get the immediate target part with lower horizontal value than our current target part
					if (tempTargetArray[i].VertVal == (CurTargetPart.VertVal - 1)) {
						//set that as our current target
						SetCurrentTargetPart(tempTargetArray[i]);
						//aim weapon at indicator
						targetLocation = TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName);
						OnSwitchedTargetPart.Broadcast();
						//end loop, we done here fuckers
						break;
					}
				}
			}
			//aim our weapon if we have a weapon
			if(currentEquippedWeapon)
				currentEquippedWeapon->SetTargetAim(currentEquippedWeapon->GetActorLocation(), TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName));
			//Disable bool flag
			CanSwitchTarget = false;
			//Delay Target Switching using boolean flags
			GetWorldTimerManager().SetTimer(SwitchTargetTimer, TimerDel, 0.25f, false);
		}
	}
}

void AAI2Character::Turn(int dir) {
	/*
	FTimerDelegate TimerDel;
	FTimerHandle SwitchTargetTimer;
	TimerDel.BindUFunction(this, FName("AllowTargetPartSwitch"), dir);
	if (CanSwitchTarget == true) {
		//translate map to array because I need to access the map by index
		TArray<FTarget> tempTargetArray;
		TargetData.GenerateValueArray(tempTargetArray);
		for (int i = 0; i < tempTargetArray.Num(); i++) {
			//We're looking for "lower" body parts
			if (dir == 1) {
				//get the immediate target part with higher horizontal value than our current target part
				if (tempTargetArray[i].HorVal == (CurTargetPart.HorVal + 1)) {
					//set that as our current target
					CurTargetPart = tempTargetArray[i];
					//end loop, we done here fuckers
					break;
				}
			}
			//We're looking for "higher" body parts
			else if (dir == -1) {
				//get the immediate target part with lower horizontal value than our current target part
				if (tempTargetArray[i].HorVal == (CurTargetPart.HorVal - 1)) {
					//set that as our current target
					CurTargetPart = tempTargetArray[i];
					//end loop, we done here fuckers
					break;
				}
			}
		}

		//Disable bool flag
		CanSwitchTarget = false;
		//Delay Target Switching using boolean flags
		GetWorldTimerManager().SetTimer(SwitchTargetTimer, TimerDel, 0.25f, false);
	}
	*/
}

void AAI2Character::AllowTargetPartSwitch(int dir) {
	CanSwitchTarget = true;
}

void AAI2Character::CoverMoveRight(float p_val) {
	coverMovingRight = (p_val < 0) ? false : true;
	FVector inverseNormal = CurrentCoverData->CoverNormal * FVector(-1.0f, -1.0f, 0.0f);
	FRotator dir = UKismetMathLibrary::MakeRotFromXZ(inverseNormal, GetCapsuleComponent()->GetUpVector());
	AddMovementInput(UKismetMathLibrary::GetRightVector(dir), p_val);
}

void AAI2Character::Jump() {
	ACharacter::Jump();
}

bool AAI2Character::CanFire() {
	if(currentEquippedWeapon!=NULL)
		if(!currentEquippedWeapon->NeedsReload() && !isReloading)
			return true;
	return false;
}

void AAI2Character::Fire() {
	if (CanFire()) {
		isFiring = true;
		//Get Enemy attention
		PawnNoiseEmitterComponent->MakeNoise(this, 1.f, GetActorLocation());

		if (CurrentTarget) {
			TargetSkeletonMesh = CurrentTarget->FindComponentByClass<USkeletalMeshComponent>();
			currentEquippedWeapon->SetTargetAim(currentEquippedWeapon->GetActorForwardVector(), TargetSkeletonMesh->GetSocketLocation(CurTargetPart.SocketName));
			currentEquippedWeapon->StartFire();
		}
		else {
			currentEquippedWeapon->StartFire();
		}
	}
	else
		StopFiring();
}

void AAI2Character::StopFiring() {
	isFiring = false;
	if(currentEquippedWeapon)
		currentEquippedWeapon->StopFire();
}

void AAI2Character::Aim() {
	if (CanAim() && !isReloading) {
		OnStartAim.Broadcast();
		isAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = AimingWalkSpeed;
		PrimitiveIndicatorDebug->SetVisibility(true, true);
		//if overlapped right on aim start
		TArray<AActor*>OverlappingActors;
		AimCollisionMesh->GetOverlappingActors(OverlappingActors, AAI2BaseNPC::StaticClass());
		int index = 0;
		if(OverlappingActors.Num() > 0)
			CurrentTarget = Cast<AAI2BaseNPC>(OverlappingActors[0]);
	}
}

void AAI2Character::ComputeForWeaponTraceForward() {
	FVector CamLoc;
	FRotator CamRot;
	if (GetController()) {
		GetController()->GetPlayerViewPoint(CamLoc, CamRot);
		FVector FinalAim = CamRot.Vector();

		FVector OutStartTrace = FVector::ZeroVector;
		FRotator DummyRot;
		GetController()->GetPlayerViewPoint(OutStartTrace, DummyRot);
		if (Instigator) {
			FVector CameraPos = OutStartTrace = OutStartTrace + FinalAim * (FVector::DotProduct((Instigator->GetActorLocation() - OutStartTrace), FinalAim));

			const FVector EndPos = CameraPos + (FinalAim * 10000);

			FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
			TraceParams.bTraceAsyncScene = true;
			TraceParams.bReturnPhysicalMaterial = true;

			FHitResult Impact(ForceInit);
			GetWorld()->LineTraceSingleByChannel(Impact, CameraPos, EndPos, ECC_GameTraceChannel2, TraceParams);

			FVector AdjustedAimDir = FinalAim;
			if (Impact.bBlockingHit) {
				/* Adjust the shoot direction to hit at the crosshair. */
				AdjustedAimDir = (Impact.ImpactPoint - currentEquippedWeapon->GetActorLocation()).GetSafeNormal();
				FHitResult Hit(ForceInit);
				GetWorld()->LineTraceSingleByChannel(Hit, currentEquippedWeapon->GetActorLocation(), currentEquippedWeapon->GetActorLocation() + (AdjustedAimDir * 10000), ECC_GameTraceChannel2, TraceParams);
				Impact = Hit;
				if(Impact.bBlockingHit)
					currentEquippedWeapon->SetTargetAim(CameraPos, Impact.Location);
			}
		}
	}
}

void AAI2Character::StopAiming() {
	OnExitAim.Broadcast();
	isAiming = false;
	//reset aiming to default part kek
	CurTargetPart = DefaultTargetPart;
	//remove current target kek
	CurrentTarget = NULL;
	//hide indicator
	PrimitiveIndicatorDebug->SetVisibility(false, true);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

bool AAI2Character::CanAim() {
	return true;
}

void AAI2Character::EnterReload() {
	//play montage
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("reload called")));
	if (currentEquippedWeapon) {
		bool isFullReload = (currentEquippedWeapon->CurrentClipSize < currentEquippedWeapon->MaxClipSize) ? false : true;
		if (ReloadMontage != NULL && !isFullReload && !isReloading && currentEquippedWeapon->CanReload()) {
			StopFiring();
			StopAiming();
			isReloading = true;
			PlayAnimMontage(ReloadMontage, 1.0f);
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("reload success")));
		}
	}
}

void AAI2Character::ReloadWeapon() {
	if(currentEquippedWeapon)
		currentEquippedWeapon->Reload();
	isReloading = false;
}

bool AAI2Character::IsCoverHigh() {
	const FName TraceTag("CoverTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("CoverTag")), true, this);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	TraceParams.TraceTag = TraceTag;
	GetWorld()->DebugDrawTraceTag = TraceTag;
	float inputRightVal = GetController()->GetInputAxisValue("MoveRight");
	FVector traceStart = GetActorLocation();
	FVector traceEnd = GetActorForwardVector() * -100;
	//if moving left, trace forward of left. Otherwise, trace forward of right
	if (inputRightVal == -1) {
		traceStart += GetActorRightVector()*2 +  FVector(0, 0.0f, 75.0f);
	}
	else {
		traceStart += -(GetActorRightVector() * 2) + FVector(0, 0.0f, 75.0f);
	}
	traceEnd += traceStart;
	
	for (int i = 0; i < 2; i++) {
		FVector offset = FVector(0, 0, 0);
		switch (i) {
		case 1:
			offset = FVector(25, 0, 0);
			break;
		case 2:
			offset = FVector(-25, 0, 0);
			break;
		default:
			offset = FVector(0, 0, 0);
			break;
		}

		GetWorld()->LineTraceSingleByChannel(
			HitData,				//result
			traceStart,	//start
			traceEnd + offset,		//end
			ECC_GameTraceChannel2,	// ECC_EngineTraceChannel2 is for Bullets
			TraceParams
		);
		if (HitData.bBlockingHit)
			return true;
	}
	return false;
}

bool AAI2Character::CanCoverMove() {
	const FName TraceTag("CoverTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("CoverTag")), true, this);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	TraceParams.TraceTag = TraceTag;
	GetWorld()->DebugDrawTraceTag = TraceTag;
	float inputRightVal = GetController()->GetInputAxisValue("MoveRight");
	FVector traceStart = GetActorLocation();
	FVector traceEnd = GetActorForwardVector() * 100;
	//if moving left, trace forward of left. Otherwise, trace forward of right
	if (inputRightVal == -1) {
		traceStart -= GetActorRightVector() * 30 + FVector(0, 0.0f, 10.0f);
	}
	else {
		traceStart += (GetActorRightVector() * 30) + FVector(0, 0.0f, 10.0f);
	}

	traceEnd += traceStart;
	GetWorld()->LineTraceSingleByChannel(
		HitData,				//result
		traceStart,	//start
		traceEnd,		//end
		ECC_GameTraceChannel2,	
		TraceParams
	);
	return HitData.bBlockingHit;
}

FHitResult AAI2Character::TraceForCover() {
	const FName TraceTag("CoverTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("CoverTag")), true, this);

	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	TraceParams.TraceTag = TraceTag;

	FVector traceStart = GetActorLocation() + FVector(0, 0, 40.0f);
	GetWorld()->DebugDrawTraceTag = TraceTag;

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector cameraForward = cameraManager->GetActorForwardVector() * 500;
	cameraForward += traceStart;
	GetWorld()->LineTraceSingleByChannel(
		HitData,				//result
		traceStart,	//start
		cameraForward,		//end
		ECC_GameTraceChannel2,	// ECC_EngineTraceChannel2 is for Bullets
		TraceParams
	);

	return HitData;
}

void AAI2Character::EnterCover() {
	//trace for cover
	FHitResult t_coverTrace = TraceForCover();

	if (t_coverTrace.bBlockingHit) {
		//compute cover location
		FVector fwd = GetActorForwardVector();
		float lx = t_coverTrace.Location.X - fwd.X;
		float ly = t_coverTrace.Location.Y - fwd.Y;
		float lz = GetActorLocation().Z;
		FVector CoverPosition = FVector(lx, ly, lz);

		//compute cover rotation
		FVector rz = GetCapsuleComponent()->GetUpVector();
		FRotator CoverRotation = UKismetMathLibrary::MakeRotFromXZ(t_coverTrace.ImpactNormal, rz);
		
		//set values
		CurrentCoverData->CoverLocation = CoverPosition;
		CurrentCoverData->CoverRotation = CoverRotation;
		CurrentCoverData->CoverNormal = t_coverTrace.ImpactNormal;
		SetAutoMoveDestination(CoverPosition);
	}
}

FVector AAI2Character::CheckForLedge() {
	FVector ledge = FVector(0, 0, 0);
	const FName TraceTag("CoverTag");
	FHitResult HitData(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("CoverTag")), true, this);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	TraceParams.TraceTag = TraceTag;
	GetWorld()->DebugDrawTraceTag = TraceTag;
	FVector traceStart = GetActorLocation();
	FVector traceEnd = GetActorForwardVector() * -100;
	traceEnd += traceStart;
	GetWorld()->LineTraceSingleByChannel(
		HitData,				//result
		traceStart,	//start
		traceEnd,		//end
		ECC_GameTraceChannel2,	// ECC_EngineTraceChannel2 is for Bullets
		TraceParams
	);

	if (HitData.bBlockingHit) {
			AActor* hitActor = HitData.Actor.Get();
			ledge.X = HitData.ImpactPoint.X;
			ledge.Y = HitData.ImpactPoint.Y;
			//Second trace, trace for Y!!
			GetWorld()->DebugDrawTraceTag = TraceTag;
			FVector traceStart = hitActor->GetActorLocation() + FVector(0,0,100);
			FVector traceEnd = hitActor->GetActorLocation();
			GetWorld()->LineTraceSingleByChannel(
				HitData,				//result
				traceStart,	//start
				traceEnd,		//end
				ECC_GameTraceChannel2,	// ECC_EngineTraceChannel2 is for Bullets
				TraceParams
			);
			ledge.Z = HitData.ImpactPoint.Z;
			DrawDebugSphere(GetWorld(), ledge, 15, 5, FColor::Green);
	}
	return ledge;
}

bool AAI2Character::CoverCheckFacingRight() {
	return coverMovingRight;
}

void AAI2Character::ToggleLockRotation(bool p_lock) {
	switch (p_lock)
	{
	case true:
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case false:
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		break;
	}
}

void AAI2Character::TryInteract() {
	AAI2GameMode* const gm = GetWorld()->GetAuthGameMode<AAI2GameMode>();
	AAI2Interactable* tInteractable = gm->GetInteractionObserver()->RequestForNearbyInteractable(this);
	if (tInteractable) {
		isInteracting = true;
		tInteractable->Interact.Broadcast();
	}
}

void AAI2Character::StopInteract() {
	AAI2GameMode* const gm = GetWorld()->GetAuthGameMode<AAI2GameMode>();
	AAI2Interactable* tInteractable = gm->GetInteractionObserver()->RequestForNearbyInteractable(this);
	if (tInteractable) {
		isInteracting = false;
		tInteractable->StopInteract.Broadcast();
	}
}

void AAI2Character::EquipWeapon() {
	//TEMPORARY CHANGE AFTER MIDTERM DEFENSE
	if (RiflePresetWeapon) {
		currentEquippedWeapon = RiflePresetWeapon;
		bUseControllerRotationYaw = true;
		ToggleLockRotation(true);
		OnAcquireWeapon.Broadcast(this);
	}
}

bool AAI2Character::HasWeapon() {
	return (currentEquippedWeapon == NULL)? false:true;
}

void AAI2Character::LockMovements() {
	bIsMovementDisabled = true;
}

void AAI2Character::UnlockMovements() {
	bIsMovementDisabled = false;
}

//interpret input from player controller
void AAI2Character::InterpretKey(FKey input) {
	if (CurrentState && !bIsMovementDisabled) {
		CurrentState->HandleInput(this, input);
	}
}

//Character HSM
void UBaseState::HandleInput(AAI2Character* character, FKey input) {
	//Turn aimed-target part (horizontal)
	if (input == EKeys::RightBracket)
		character->Turn(1);
	else if (input == EKeys::LeftBracket)
		character->Turn(-1);

	//Turn aimed-target part (vertical)
	if (input == EKeys::PageUp)
		character->LookUp(1);
	else if (input == EKeys::PageDown)
		character->LookUp(-1);

	if (input == EKeys::C) {
		//check if cover exists first
		FHitResult tTraceResult = character->TraceForCover();
		if (tTraceResult.bBlockingHit && tTraceResult.Actor->ActorHasTag("Coverable")) {
			character->EnterCover();
			character->SetCharacterState<UAutoMoveState>();
			return;
		}
		else
			character->SetCharacterState<UBase_CrouchState>();
	}

	//else if (input == EKeys::C)
	//	character->SetCharacterState<UBase_CrouchState>();
	else if (input == EKeys::LeftShift) {
		if(!(character->isAiming || character->isFiring))
			character->SetCharacterState<UBase_SprintState>();
	}
	else if (input == EKeys::SpaceBar)
		character->Jump();
	else if (input == EKeys::F)
		character->TryInteract();
	else
		character->SetCharacterState<UBase_IdleWalkState>();
}

void UBase_IdleWalkState::HandleInput(AAI2Character* character, FKey input) {
	//Turn aimed-target part (horizontal)
	if (input == EKeys::RightBracket)
		character->Turn(1);
	else if (input == EKeys::LeftBracket)
		character->Turn(-1);

	//Turn aimed-target part (vertical)
	if (input == EKeys::PageUp)
		character->LookUp(1);
	else if (input == EKeys::PageDown)
		character->LookUp(-1);

	if (input == EKeys::W)
		character->MoveForward(1);
	else if (input == EKeys::S)
		character->MoveForward(-1);
	else if (input == EKeys::D)
		character->MoveRight(1);
	else if (input == EKeys::A)
		character->MoveRight(-1);
	else
		UBaseState::HandleInput(character, input);
}

void UBase_SprintState::HandleInput(AAI2Character* character, FKey input) {
	if (input == EKeys::W)
		character->MoveForward(1);
	else if (input == EKeys::A)
		character->MoveRight(-1);
	else if (input == EKeys::D)
		character->MoveRight(1);
	else if (input == EKeys::S)
		character->MoveForward(-1);
	else
		UBaseState::HandleInput(character, input);
}

void UBase_SprintState::Update(AAI2Character* character, float deltaTime) {
	//if stopped moving, stop sprinting
	if (character->GetVelocity().IsZero())
		character->SetCharacterState<UBaseState>();
	if (character->isFiring) {
		character->SetCharacterState<UBase_IdleWalkState>();
	}
}

void UBase_CrouchState::HandleInput(AAI2Character* character, FKey input) {
	//Turn aimed-target part (horizontal)
	if (input == EKeys::RightBracket)
		character->Turn(1);
	else if (input == EKeys::LeftBracket)
		character->Turn(-1);

	//Turn aimed-target part (vertical)
	if (input == EKeys::PageUp)
		character->LookUp(1);
	else if (input == EKeys::PageDown)
		character->LookUp(-1);

	if (input == EKeys::W)
		character->MoveForward(1);
	else if (input == EKeys::S)
		character->MoveForward(-1);
	else if (input == EKeys::D)
		character->MoveRight(1);
	else if (input == EKeys::A)
		character->MoveRight(-1);
	else if (input == EKeys::C) {
		FHitResult tTraceResult = character->TraceForCover();
		if (character->HasWeapon()) {
			if (tTraceResult.bBlockingHit && tTraceResult.Actor->ActorHasTag("Coverable")) {
				character->EnterCover();
				character->SetCharacterState<UAutoMoveState>();
				return;
			}
			else
				character->SetCharacterState<UBaseState>();
		}
		else
			character->SetCharacterState<UBaseState>();
	}
	else
		UBaseState::HandleInput(character, input);
}

void UAutoMoveState::HandleInput(AAI2Character* character, FKey input) {
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, (TEXT("CharacterState: Autorun...")));
}

//only a single call state, handles rotation lock and cover searching by entry-exit functions
void UCoverState::HandleInput(AAI2Character* character, FKey input) {
	if(input != EKeys::AnyKey)
		character->SetCharacterState<UBaseState>();
}

//this acts as superclass instead
void UCover_IdleWalkState::HandleInput(AAI2Character* character, FKey input) {
	if (character->CanCoverMove()) {
		if(!character->IsCoverHigh())
			character->SetCharacterState<UCover_CrouchState>();
		if (input == EKeys::D)
			character->CoverMoveRight(1);
		else if (input == EKeys::A)
			character->CoverMoveRight(-1);
		else if (input == EKeys::F)
			character->TryInteract();
		else
			UCoverState::HandleInput(character, input);
	}
	else
		character->SetCharacterState<UCover_PeekState>();
}

void UCover_PeekState::HandleInput(AAI2Character* character, FKey input) {
	if (character->CanCoverMove())
		character->SetCharacterState<UCover_IdleWalkState>();
	if (input == EKeys::F) {
		//check if cover exists first
		if (character->TraceForCover().bBlockingHit) {
			character->EnterCover();
			character->SetCharacterState<UAutoMoveState>();
			return;
		}
	}
	else if (input == EKeys::S) {
		character->ToggleLockRotation(false);
		character->SetCharacterState<UBase_IdleWalkState>();
	}
}

void UCover_CrouchState::HandleInput(AAI2Character* character, FKey input) {
	if (!character->IsCoverHigh()) {
		if (character->CanCoverMove()) {
			if (input == EKeys::D)
				character->CoverMoveRight(1);
			else if (input == EKeys::A)
				character->CoverMoveRight(-1);
			else if (input == EKeys::S) {
				character->ToggleLockRotation(false);
				character->SetCharacterState<UBase_IdleWalkState>();
			}
			else if (input == EKeys::F) {
				character->SetCharacterState<UVaultState>();
			}
		}
		else
			character->SetCharacterState<UCover_PeekState>();
	}
	else 
		character->SetCharacterState<UCover_IdleWalkState>();
}

//State Transitions
void UBaseState::OnEnter(AAI2Character* character) {
	character->ToggleLockRotation(false);
}

void UBaseState::OnExit(AAI2Character* character) {
	
}

void UBase_SprintState::OnEnter(AAI2Character* character) {
	character->EnterSprint();
}

void UBase_SprintState::OnExit(AAI2Character* character) {
	character->ExitSprint();
}

void UBase_CrouchState::OnEnter(AAI2Character* character) {
	if (!character->currentEquippedWeapon) {
		character->bUseControllerRotationYaw = false;
		character->ToggleLockRotation(false);
	}
	character->EnterCrouch();
}

void UBase_CrouchState::OnExit(AAI2Character* character) {
	if (character->currentEquippedWeapon) {
		character->bUseControllerRotationYaw = true;
		character->ToggleLockRotation(true);
	}
	character->ExitCrouch();
}

void UCoverState::OnEnter(AAI2Character* character) {
	//move the root object to cover location and rotate
	FLatentActionInfo actionInfo = FLatentActionInfo();
	actionInfo.CallbackTarget = character;
	UKismetSystemLibrary::MoveComponentTo(character->GetCapsuleComponent(), 
		character->GetCurrentCoverData()->CoverLocation, 
		character->GetCurrentCoverData()->CoverRotation - FRotator(0, 180, 0),
		false, true, 0.2f, false, EMoveComponentAction::Move, actionInfo);
	character->ToggleLockRotation(true);
	character->SetCharacterState<UCover_IdleWalkState>();
}

void UCoverState::OnExit(AAI2Character* character) {
	
}

void UCover_CrouchState::OnEnter(AAI2Character* character) {
	character->EnterCrouch();
}

void UCover_CrouchState::OnExit(AAI2Character* character) {
	character->ExitCrouch();
}

void UAutoMoveState::OnEnter(AAI2Character* character) {
	character->ToggleLockRotation(false);
	character->EnterCover();
}

void UAutoMoveState::OnExit(AAI2Character* character) {
	
}

void UVaultState::OnEnter(AAI2Character* character) {
	character->ToggleLockRotation(false);
	character->OnStartVault.Broadcast();
	character->SetActorEnableCollision(false);
	FVector tempLoc = FVector(character->GetActorLocation().X, character->GetActorLocation().Y - 10, character->CheckForLedge().Z);
	character->SetActorLocation(tempLoc);
	character->SetActorRotation(character->GetActorRotation() + FRotator(0, 180, 0));
}

void UVaultState::OnExit(AAI2Character* character) {
	character->SetActorEnableCollision(true);
}

//Update Nodes
void UAutoMoveState::Update(AAI2Character* character, float deltaTime) {
	if (!character->bAutoDestinationReached)
		character->RunAutoMove();
	else
		character->SetCharacterState<UCoverState>();
}

void UVaultState::Update(AAI2Character* character, float deltaTime) {
	character->SetActorLocation(character->GetActorLocation() + character->GetActorForwardVector() * deltaTime * 150.0f);
	if (!(character->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())) {
		character->SetCharacterState<UBaseState>();
	}
}