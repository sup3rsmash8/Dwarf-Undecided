// Fill out your copyright notice in the Description page of Project Settings.
// MAIN SCRIPT AUTHOR: Justus
// Secondary Author: Johan Liljedahl
// Secondary Author: Hjalmar Andersson


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoolGP3Project/CustomComponents/Movement/GeneralMovementComponent.h"
#include "GameSound/FootstepComponent.h"
#include "GameSystem/GP_GameInstance.h"
#include "CustomComponents/ItemCarryPositionComponent.h"
#include "Item/BaseItem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CustomComponents/InteractableComponent.h"
#include "CustomComponents/ItemHolderComponent.h"
#include "Item/GoldChest.h"
#include "Journal/PlayerJournal.h"
#include "GameSystem/GM_DayCycleComponent.h"

//UI
#include "UI/HUD/GameHUD.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetCapsuleSize(45.f, 96.f);
	CapsuleComponent->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = CapsuleComponent;

	HeadSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Head Socket"));
	HeadSocket->SetupAttachment(CapsuleComponent);
	HeadSocket->SetRelativeLocation(FVector(0, 0, CapsuleComponent->GetScaledCapsuleHalfHeight_WithoutHemisphere()));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(HeadSocket);
	Camera->SetRelativeLocation(FVector::ZeroVector);

	CarryPosition = CreateDefaultSubobject<USceneComponent>(TEXT("CarryPosition")); //JL
	CarryPosition->SetupAttachment(HeadSocket); //JL

	MovementComp = CreateDefaultSubobject<UGeneralMovementComponent>(TEXT("Movement Component"));

	FootstepComponent = CreateDefaultSubobject<UFootstepComponent>(TEXT("Footstep Component"));

	//TArray<FInputAxisBinding> BindAxes
	//{
	//	(PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp)),
	//	(PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight)),
	//	(PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward)),
	//	(PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight)),
	//};
	//TArray<FInputActionBinding> BindActions
	//{
	//	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::InteractWithItem), //JL
	//	PlayerInputComponent->BindAction("Break", IE_Pressed, this, &APlayerCharacter::BreakInteract),
	//	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &APlayerCharacter::OpenMenu),
	//	//PlayerInputComponent->BindAction("NextPage", IE_Released, this, &APlayerCharacter::NextPage),
	//	//PlayerInputComponent->BindAction("PreviousPage", IE_Released, this, &APlayerCharacter::PrevPage),
	//};
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Cam = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass())); //JL
	Touch = Cast<UItemCarryPositionComponent>(GetComponentByClass(UItemCarryPositionComponent::StaticClass())); //JL

	APlayerMultiController* MyPMC = GetController<APlayerMultiController>();
	if (MyPMC)
	{
		MyPMC->OnInteract.AddDynamic(this, &APlayerCharacter::InteractWithItem);
		MyPMC->OnBreak.AddDynamic(this, &APlayerCharacter::BreakInteract);
		MyPMC->OnMenu.AddDynamic(this, &APlayerCharacter::OpenMenu);

		MyPMC->OnMoveForward.AddDynamic(this, &APlayerCharacter::MoveForward);
		MyPMC->OnMoveRight.AddDynamic(this, &APlayerCharacter::MoveRight);
		MyPMC->OnLookUp.AddDynamic(this, &APlayerCharacter::LookUp);
		MyPMC->OnLookRight.AddDynamic(this, &APlayerCharacter::LookRight);
	}

	BeginPlayPosition = GetActorLocation();
	auto* GM = Cast<AGP3GameMode>(UGameplayStatics::GetGameMode(this));
	if (GM == nullptr)
		return;
	DayComponent = GM->GetDayCycleComponent();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUseDeathFloor)
	{
		const float ZPos = GetActorLocation().Z;
		HandleDeathFloor(ZPos);
	}

	//HJ
	if (bRestrictMovement)
		return;

	if (bRotationIsControllerRotation && Controller)
	{
		if (Controller)
		{
			const FRotator ControllerRotation = Controller->GetControlRotation();
			const FRotator CYaw(0, ControllerRotation.Yaw, 0);
			const FRotator CPitch(ControllerRotation.Pitch, 0, 0);
			
			SetActorRotation(CYaw);
			HeadSocket->SetRelativeRotation(CPitch);
		}
	}
	
	//////////////////////////////////////////////////////////////// JL ----->
	Hands.SetLocation(Cam->GetComponentLocation());
	Hands.SetRotation(Cam->GetComponentQuat());
	Origin = Hands.GetLocation();
	End = Hands.GetLocation() + Cam->GetForwardVector() * 200;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_WorldDynamic); 
	UKismetSystemLibrary::LineTraceSingle(this, Origin, End, TraceType, false, IgnoreList, EDrawDebugTrace::None, Hit, true);
	//HJ
	if (Hit.bBlockingHit && Hit.GetActor() != nullptr)
	{
		UInteractableComponent* Interact = Hit.GetActor()->FindComponentByClass<UInteractableComponent>();
		if(Interact)
		{
			UpdateInteract(Interact);
		}
		else if(CurrentInteract != nullptr)
		{
			UpdateInteract(nullptr);
		}
	}
	else if(!Hit.bBlockingHit && CurrentInteract != nullptr)
	{
		UpdateInteract(nullptr);
	}
	//
	//////////////////////////////////////////////////////////////// JL <-----
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//TArray<FInputAxisBinding> BindAxes
	//{
	//	(PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp)),
	//	(PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight)),
	//	(PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward)),
	//	(PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight)),
	//};
	//TArray<FInputActionBinding> BindActions
	//{
	//	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::InteractWithItem), //JL
	//	PlayerInputComponent->BindAction("Break", IE_Pressed, this, &APlayerCharacter::BreakInteract),
	//	PlayerInputComponent->BindAction("Menu", IE_Pressed, this, &APlayerCharacter::OpenMenu),
	//	//PlayerInputComponent->BindAction("NextPage", IE_Released, this, &APlayerCharacter::NextPage),
	//	//PlayerInputComponent->BindAction("PreviousPage", IE_Released, this, &APlayerCharacter::PrevPage),
	//};

	//// Prevent input consumption so we can feed the poor (i.e. let other objects read input too)
	////for (FInputAxisBinding& Bind : BindAxes) { Bind.bConsumeInput = false; }
	////for (FInputActionBinding& Bind : BindActions) { Bind.bConsumeInput = false; }

}

void APlayerCharacter::InteractWithItem() //JL
{
	// Prevent interaction if we're restricted (in a cutscene, etc.)
	if (IsRestricted())
	{
		return;
	}

	APlayerController* PlayerCtrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AGameHUD* GameHUD = Cast<AGameHUD>(PlayerCtrl->GetHUD());
	//GameHUD->GetDialogWidget()->OnPromptPressConfirm();
	if (CurrentInteract != nullptr && !CurrentInteract->Interacted) 
	{
		CurrentInteract->OnPlayerInteract.Broadcast(this);
	}

	AActor* HitActor = Hit.GetActor();
	if (HitActor != nullptr)
	{
		// Johan - vfn
		if (HitActor->GetClass()->IsChildOf(ABaseItem::StaticClass())) //JL Raycast hit an item
		{
			IgnoreList.Empty();
			IgnoreList.Add(Cast<AActor>(HitActor));
			Touch->InteractWithItem(HitActor);
		}
		else if (HitActor->FindComponentByClass(UItemHolderComponent::StaticClass())) //JL Raycast hit a itemHolder
		{
			IgnoreList.Empty();
			Touch->InteractWithCounter(HitActor);
		}
		else if (HitActor->IsA(AGoldChest::StaticClass()))
		{
			IgnoreList.Empty();
			Touch->InteractWithGoldChest(HitActor);
		}
	}
}

USceneComponent* APlayerCharacter::GetCarryPosition()
{
	return CarryPosition;
}

float APlayerCharacter::GetMouseSensitivity() const
{
	if (UWorld* World = GetWorld())
	{
		if (auto* Inst = Cast<UGP_GameInstance>(UGameplayStatics::GetGameInstance(World)))
		{
			return Inst->GetGameSettings()->MouseSensitivity;
		}
	}

	return 1.0f;
}

void APlayerCharacter::SetMouseSensitivity(float Value) const
{
	if (UWorld* World = GetWorld())
	{
		if (auto* Inst = Cast<UGP_GameInstance>(UGameplayStatics::GetGameInstance(World)))
		{
			Inst->SetMouseSense(Value);
		}
	}
}

USceneComponent* APlayerCharacter::GetHeadSocket()
{
	return HeadSocket;
}

void APlayerCharacter::RestrictMovementInput(bool bRestrict)
{
	const bool bPrevRestrict = bRestrictMovement;
	bRestrictMovement = bRestrict;

	if (bRestrictMovement != bPrevRestrict)
	{
		if (bRestrictMovement)
		{
			OnPlayerRestricted();
		}
		else
		{
			OnPlayerUnrestricted();
		}
	}
}

void APlayerCharacter::OnPlayerRestricted()
{
	if (MovementComp)
		MovementComp->ConsumeInputVector();
}

void APlayerCharacter::OnPlayerUnrestricted()
{
}

void APlayerCharacter::SetPlayerControllerInactiveState(bool bInactive)
{
	//APlayerMultiController* MyPMC = GetController<APlayerMultiController>();

	//if (!MyPMC)
	//	return;

	//if (bInactive)
	//{
	//	MyPMC->ChangeState(NAME_Inactive);
	//	//MyPMC->SetInputMode
	//}
	//else
	//{
	//	MyPMC->ChangeState(NAME_Playing);
	//}
}

void APlayerCharacter::AssignJournal(APlayerJournal* NewJournal)
{
	Journal = NewJournal;
}

void APlayerCharacter::LookUp(float Value)
{
	//HJ
	if (bRestrictMovement)
		Value = 0;

	AddControllerPitchInput(Value * BaseLookUpRate * GetMouseSensitivity() * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookRight(float Value)
{
	//HJ
	if (bRestrictMovement)
		Value = 0;

	AddControllerYawInput(Value * BaseLookRightRate * GetMouseSensitivity() * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	//HJ
	if (bRestrictMovement)
		Value = 0;

	FVector DirVector = FVector::ForwardVector;//GetActorForwardVector()

	const FVector AddForward = DirVector * Value;
	AddMovementInput(AddForward);
}

void APlayerCharacter::MoveRight(float Value)
{
	//HJ
	if (bRestrictMovement)
		Value = 0;

	FVector DirVector = FVector::RightVector;//GetActorForwardVector()

	const FVector AddRight = DirVector * Value;
	AddMovementInput(AddRight);
}

void APlayerCharacter::HandleDeathFloor(float ZPosition)
{	
	// Avoid doing stuff if the WorldSettings' death floor already has consumed us
	if (IsPendingKillPending())
		return;

	const float DefaultZ = -200.f;

	float DeathFloorZ;
	if (bUseCustomDeathFloorValue)
	{
		DeathFloorZ = CustomDeathFloorZ;
	}
	else
	{
		UWorld* World = GetWorld();

		if (World)
		{
			AWorldSettings* WorldSettings = World->GetWorldSettings();
			DeathFloorZ = WorldSettings ? WorldSettings->KillZ : DefaultZ;
		}
		else
		{
			DeathFloorZ = DefaultZ;
		}
	}


	if (ZPosition <= DeathFloorZ)
	{
		if (MovementComp)
			MovementComp->Velocity = FVector::ZeroVector;

		SetActorLocation(BeginPlayPosition);
	}
}

void APlayerCharacter::BreakInteract()
{
	if(CurrentInteract != nullptr && CurrentInteract->Interacted)
	{
		CurrentInteract->OnPlayerBreakInteract.Broadcast(this);
	}
}

void APlayerCharacter::OpenMenu()
{
	if (bRestrictMovement)
		return;
	if(CurrentInteract == nullptr || CurrentInteract != nullptr && !CurrentInteract->Interacted)
	{
		if (MenuActive)
			return;
		RestrictMovementInput(true);
		ActivateMenu();
		DisplayNote(true);
		MenuActive = true;
	}
	else if (CurrentInteract->Interacted)
	{
		CurrentInteract->OnPlayerBreakInteract.Broadcast(this);
	}
}

void APlayerCharacter::CloseMenu()
{
	DisplayNote(false);
	MenuActive = false;
}

/// <summary>
/// HJ
/// This Function checks the current interacted object and sees if it should hover or not.
/// Also if it should replace the current interact with something else
/// </summary>
/// <param name="Interact">The new InteractableComponent</param>
void APlayerCharacter::UpdateInteract(UInteractableComponent* Interact)
{
	if (CurrentInteract == nullptr && Interact != nullptr)
	{
		CurrentInteract = Interact;
		CurrentInteract->OnPlayerHover.Broadcast(this);
	}
	else if(Interact == nullptr)
	{
		CurrentInteract->OnPlayerStopHover.Broadcast(this);
		CurrentInteract = nullptr;
	}
	else if(CurrentInteract != Interact)
	{
		CurrentInteract->OnPlayerStopHover.Broadcast(this);
		CurrentInteract = Interact;
		CurrentInteract->OnPlayerHover.Broadcast(this);
	}
}

void APlayerCharacter::SendDayState(EDayState State)
{
	DayComponent->AfterNews();
}