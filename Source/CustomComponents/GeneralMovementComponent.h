// Fill out your copyright notice in the Description page of Project Settings.

// MAIN SCRIPT AUTHOR: Justus

#pragma once

#include "CoreMinimal.h"
#include "CharMovementData.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GeneralMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGroundStateChangedDelegate, const bool, OnGround, const bool, PreviouslyOnGround);

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UGeneralMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UGeneralMovementComponent();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Delegate that gets called when the ground state changes (when the component lands on ground or leaves it). */
	FGroundStateChangedDelegate OnGroundStateChanged;
	
protected:
	/* The surface normal of the last ground stood on. */
	FVector GroundNormal;
	
	/* If the player is standing on solid ground or not. */
	UPROPERTY(VisibleAnywhere)
	bool bOnGround;
	
	/* The rate at which the owner of this actor moves. In U/s. */
	UPROPERTY(VisibleAnywhere)
	FVector CompVelocity;

	/* Vector that CompVelocity uses as reference in conjunction with input. */
	FVector VelocityInput;

	FVector MoveAxes;
	
	/* The component's movement data, as in speed and gravity, etc. */
	UPROPERTY(EditAnywhere)
		UCharMovementData* MovementData;
	/* The threshold for where slopes start counting as walls. */
	UPROPERTY(EditDefaultsOnly)
		float MaxSlopeAngle = 55.f;
	
	/* Movement updates and collision detection are executed in here. */
	virtual void ExecuteMovement(FVector& TestVelocity, bool& TestOnGround, float DeltaTime);

	/* Returns the next speed that CompVelocity is supposed to receive, based on the values in MovementData. */
	UFUNCTION(BlueprintCallable)
		virtual void CalculateNextVelocity(FVector& TargetVelocity, const FVector& Input, float DeltaTime);

public:
	/* Returns the surface normal value, if standing on ground and the vector is a valid normal (in case it would be zero). */
	UFUNCTION(BlueprintCallable)
		bool GetSurfaceNormal(FVector& OutNormal);

	/* Returns the current rate at which the component moves. In U/s. */
	UFUNCTION(BlueprintCallable)
		FVector GetVelocity() const { return CompVelocity; }


	
};
