// Fill out your copyright notice in the Description page of Project Settings.

// MAIN SCRIPT AUTHOR: Justus
// Secondary Author: Johan Liljedahl
// Secondary Author: Hjalmar Andersson

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameSystem/PlayerMultiController.h"
#include "GameSystem/DayStateDelegate.h"
#include "PlayerCharacter.generated.h"

class UGeneralMovementComponent;
class UCapsuleComponent;
class UCameraComponent;
class UItemCarryPositionComponent;
class UFootstepComponent;
//HJ
class UInteractableComponent;
class APlayerJournal;
class UGM_DayCycleComponent;

UCLASS()
class COOLGP3PROJECT_API APlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCharacter();
	UCameraComponent* Cam; //JL
	FTransform Hands;//JL
	FVector Origin;
	FVector End;

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateMenu();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisplayNote(bool Status);
	UFUNCTION(BlueprintCallable)
	void CloseMenu();

	UFUNCTION(BlueprintCallable)
	void SendDayState(EDayState State);

protected:
	/* The movement component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UGeneralMovementComponent* MovementComp;

	/* The camera, the player's eyes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* Camera;

	/* The component that represents the head. Camera, hand animations, etc. go here. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* HeadSocket;
	
	/* Where the actor is carrying items from, The actors hands */ //JL
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CarryPosition;

	/* The movement component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UFootstepComponent* FootstepComponent;

	UGM_DayCycleComponent* DayComponent;

	UPROPERTY(EditDefaultsOnly)
		float BaseLookUpRate = 45.f;

	UPROPERTY(EditDefaultsOnly)
		float BaseLookRightRate = 45.f;

	/* If true, this actor will treat a custom value as its death floor threshold. Otherwise, use the world's death floor value. */
	UPROPERTY(EditAnywhere)
		bool bUseCustomDeathFloorValue = true;

	/* If our Z-axis value goes below this number, respawn us at BeginPlayPosition.  */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseCustomDeathFloorValue"))
		float CustomDeathFloorZ = -200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bRotationIsControllerRotation = true;
	
	FHitResult Hit; //JL
	TArray<AActor*> IgnoreList;
	UItemCarryPositionComponent* Touch; //JL

	/* Useless debug feature, sets whether or not we should be consumed by the death floor. */
	bool bUseDeathFloor = true;

	/* The position this actor had when BeginPlay was called. Used for failsafes to put you back in-bounds when reaching the death floor. */
	FVector BeginPlayPosition;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void MoveForward(float Value);
	UFUNCTION()
	virtual void MoveRight(float Value);
	UFUNCTION()
	virtual void LookUp(float Value);
	UFUNCTION()
	virtual void LookRight(float Value);

	/* Handles events that take place when the player is beneath the death floor. */
	UFUNCTION()
	virtual void HandleDeathFloor(float ZPosition);
	
	//HJ
	UPROPERTY(EditAnywhere, Category = "A Interactive")
	bool bInteracted = false;
	UPROPERTY(EditAnywhere, Category = "A Interactive")
	UInteractableComponent* CurrentInteract;
	APlayerJournal* Journal;

	//HJ
	UPROPERTY(EditAnywhere, Category = "A Interactive")
	bool bRestrictMovement = false;
	bool MenuActive = false;

	void OnPlayerRestricted();
	void OnPlayerUnrestricted();
public:
	UFUNCTION(BlueprintCallable)
	/* Restricts or unrestricts player inputs. */
	void RestrictMovementInput(bool bRestrict);

	UFUNCTION(BlueprintCallable)
	bool IsRestricted() const { return bRestrictMovement; }

	UFUNCTION(BlueprintCallable)
	/* Sets the player's PlayerController state, allowing input to be executed on menu-related elements. */
	void SetPlayerControllerInactiveState(bool bInactive);

	//HJ
	void AssignJournal(APlayerJournal* NewJournal);
	USceneComponent* GetHeadSocket();

	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//HJ
	void UpdateInteract(UInteractableComponent* Interact);
	UFUNCTION(BlueprintCallable)
	void BreakInteract();
	UFUNCTION()
	void OpenMenu();

	UFUNCTION()
	void InteractWithItem(); //JL
	USceneComponent* GetCarryPosition(); //JL
	FVector CameraPos; // JL

	UFUNCTION(BlueprintCallable)
	/* Retrieves the mouse sensitivity this character uses. */
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float Value) const;
};
