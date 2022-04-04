// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharMovementData.generated.h"

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UCharMovementData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/* The top horizontal speed the character can reach, in U/s. */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float MoveSpeed;
	/* How many U/s the speed increases by when moving. */
	UPROPERTY(EditAnywhere, Category = "Speed", meta=(ClampMin="0.0"))
	float Acceleration;
	/* The vertical speed applied when pressing the jump button. */
	UPROPERTY(EditAnywhere, Category = "Speed", meta = (ClampMin = "0.0"))
	float JumpPower;
	/* The downwards acceleration applied when in mid-air, in U/s2. */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float Gravity;

	//UPROPERTY(EditAnywhere, Category = "Turning")
	//float YAxisTurnSpeed;

	//UPROPERTY(EditAnywhere, Category = "Turning")
	//float ZAxisTurnSpeed;
};
