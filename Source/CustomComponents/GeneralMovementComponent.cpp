// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomComponents/Movement/GeneralMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGeneralMovementComponent::UGeneralMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bUseAccelerationForPaths = true;
}

void UGeneralMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGeneralMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector& Vel = Velocity;

	FVector Input = ConsumeInputVector();
	CalculateNextVelocity(Vel, Input, DeltaTime);

	bool bPreviousOnGround = bOnGround;

	ExecuteMovement(Vel, bOnGround, DeltaTime);

	//UE_LOG(LogTemp, Log, TEXT("(%f, %f)"), Vel.X, Vel.Y);

	// Report that the grounded state has changed, if it did.
	if (bPreviousOnGround != bOnGround)
	{
		OnGroundStateChanged.Broadcast(bOnGround, bPreviousOnGround);
	}
	
	if (bOnGround && Vel.Z < 0)
		Vel.Z = 0;
}

bool UGeneralMovementComponent::GetSurfaceNormal(FVector& OutNormal)
{
	if (!bOnGround)
		return false;
	
	FVector Normal = GroundNormal.GetSafeNormal();

	bool bIsValid = Normal.IsNormalized();
	
	if (bIsValid)
	{
		OutNormal = Normal;
	}

	return bIsValid;
}

void UGeneralMovementComponent::CalculateNextVelocity(FVector& TargetVelocity, const FVector& Input, float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!MovementData || !Owner)
		return;

	float FrameMoveSpeed = MovementData->MoveSpeed;
	float FrameAcceleration = MovementData->Acceleration;
	float FrameGravity = MovementData->Gravity;
	
	FVector InputVector = Input;
	InputVector = FVector(InputVector.X, InputVector.Y, 0);
	MoveAxes = InputVector * FrameMoveSpeed;//FMath::VInterpConstantTo(MoveAxes, InputVector * FrameMoveSpeed, DeltaTime, FrameAcceleration * FrameAcceleration * DeltaTime);

	FVector Fwd = Owner->GetActorForwardVector() * MoveAxes.X;
	FVector Rgt = Owner->GetActorRightVector() * MoveAxes.Y;
	FVector NextVelocity = FMath::VInterpConstantTo(TargetVelocity, (Fwd + Rgt), DeltaTime, FrameAcceleration * FrameAcceleration * DeltaTime); //(Fwd + Rgt);
	
	if (bOnGround)
	{
		TargetVelocity = FVector(NextVelocity.X, NextVelocity.Y, TargetVelocity.Z);
	}	
	else
	{
		//FVector LerpToNextVelocity = InputVector * FrameMoveSpeed;
		const float VelZ = TargetVelocity.Z;
		TargetVelocity = UKismetMathLibrary::VInterpTo_Constant(FVector(TargetVelocity.X, TargetVelocity.Y, 0), NextVelocity, DeltaTime, FrameAcceleration * FrameAcceleration * .4f * DeltaTime);
		TargetVelocity.Z = VelZ - FrameGravity * DeltaTime;
	}

	//const float AccelMult = .4f;
	//
	//const float VelZ = TargetVelocity.Z;

	//TargetVelocity = UKismetMathLibrary::VInterpTo_Constant(FVector(TargetVelocity.X, TargetVelocity.Y, 0), NextVelocity, 
	//	DeltaTime, FrameAcceleration * FrameAcceleration * AccelMult * DeltaTime);

	//TargetVelocity.Z = VelZ - FrameGravity * DeltaTime;
}

void UGeneralMovementComponent::ExecuteMovement(FVector& TestVelocity, bool& TestOnGround, float DeltaTime)
{
	if (!MovementData)
		return;

	APawn* Owner = GetPawnOwner();
	if (!Owner || Owner->IsPendingKill())
		return;

	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Owner->GetComponentByClass(UCapsuleComponent::StaticClass()));
	if (!Capsule)
		return;

	FHitResult Hit;
	FHitResult ZHit;
	// Is applied at the end of the function to &TestOnGround.
	bool DidTouchGround = false;

	//UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), GetActorLocation(), Capsule->GetScaledCapsuleHalfHeight(), Capsule->GetScaledCapsuleRadius(), GetActorRotation(), FLinearColor::Red, 0, 1);

	// The translation normal. Used for proper horizontal movement along surfaces.
	FVector ProjectNormal = TestOnGround ? GroundNormal : FVector::ZeroVector;

	// The horizontal movement this frame.
	FVector Translation(FVector::VectorPlaneProject(FVector(TestVelocity.X, TestVelocity.Y, 0), ProjectNormal));
	bool bApplyToTestSpeed = false;

#pragma region Vertical
	// The vertical movement this frame.
	FVector ZTranslation;
	if (TestOnGround)
	{
		// When on ground, we want to add some downwards velocity so we can sweep
		// to check if we have ground beneath us. Of course, since this invalidates
		// jumping, we only do this if our velocity is less than or equal to 0.
		if (TestVelocity.Z > 0)
			ZTranslation = FVector(0, 0, TestVelocity.Z);
		else
			ZTranslation = FVector::DownVector * MovementData->Gravity;
	}
	else
	{
		ZTranslation = FVector(0, 0, TestVelocity.Z);
	}

	Owner->AddActorWorldOffset(ZTranslation * DeltaTime, true, &ZHit);

	if (ZHit.bBlockingHit)
	{
		const float ResultAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(ZHit.Normal, FVector::UpVector));
		if (ResultAngle < MaxSlopeAngle)
		{
			if (!DidTouchGround)
			{
				const float MaxSlopeRadAngle = FMath::DegreesToRadians(MaxSlopeAngle);

				// We perform a linetrace to make sure that there's nothing
				// below us, since there's a discrepancy between collision
				// on stairs and collision by edges. If there's nothing directly
				// below us, we should just start falling, even if we're barely
				// touching the edge at the same time.
				FVector Start = GetActorLocation() + FVector::DownVector * Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();

				// Line trace should end at the furthest reachable point with
				// the character's slope limit. 
				FVector EndOffset = FVector::DownVector * (Capsule->GetScaledCapsuleRadius() + Capsule->GetScaledCapsuleRadius() * FMath::FastAsin(MaxSlopeRadAngle));
				FVector End = Start + EndOffset;

				ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_WorldDynamic);
				TArray<AActor*> Ignore;
				// bIgnoreSelf isn't working somehow...
				Ignore.Add(Owner);

				FHitResult LineHit;
				EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;

				DidTouchGround = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceType, true, Ignore, DebugTrace, LineHit, true);
			}
		}

		if (ResultAngle >= MaxSlopeAngle || !DidTouchGround)
		{
			// Recursive check to push us out of any possible
			// collision.
			int IterationLimit = 5;
			while (IterationLimit > 0 && !ZTranslation.IsNearlyZero())
			{
				// If we somehow end up intersecting with a surface,
				// push us out before continuing on, because otherwise
				// we might be rendered stuck.
				if (ZHit.bStartPenetrating)
				{
					int PeneIterationLimit = 5;
					FHitResult PeneHit = ZHit;
					while (PeneIterationLimit > 0 && PeneHit.bStartPenetrating)
					{
						// Push us out along the normal, according to how deep
						// into the surface we are.
						FVector Pushout = PeneHit.Normal * (PeneHit.PenetrationDepth + 1.0001f);
						Owner->AddActorWorldOffset(Pushout, true, &PeneHit);

						PeneIterationLimit--;
					}
				}

				FVector HitNormal = ZHit.Normal;
				// Project along the wall slope.
				ZTranslation = FVector::VectorPlaneProject(ZTranslation, HitNormal);

				Owner->AddActorWorldOffset(ZTranslation * DeltaTime, true, &ZHit);

				// Exit prematurely if we didn't collide with anything this sweep
				if (!ZHit.bBlockingHit)
					break;

				IterationLimit--;
			}
		}
	}
#pragma endregion 

#pragma region Horizontal
	Owner->AddActorWorldOffset(Translation * DeltaTime, true, &Hit);

	if (Hit.bBlockingHit)
	{
		// Same iterative process as in vertical.
		int IterationLimit = 5;
		while (IterationLimit > 0 && !Translation.IsNearlyZero())
		{
			// (If we somehow end up intersecting with a surface,
			// push us out before continuing on, because otherwise
			// we might be rendered stuck.)
			if (Hit.bStartPenetrating)
			{
				int PeneIterationLimit = 5;
				FHitResult PeneHit = Hit;
				while (PeneIterationLimit > 0 && PeneHit.bStartPenetrating)
				{
					FVector Pushout = PeneHit.Normal * (PeneHit.PenetrationDepth + 1.0001f);
					Owner->AddActorWorldOffset(Pushout, PeneIterationLimit > 1, &PeneHit);

					PeneIterationLimit--;
				}
			}

			FVector HitNormal = Hit.Normal;

			const float ResultAngle = UKismetMathLibrary::DegAcos(FVector::DotProduct(HitNormal, FVector::UpVector));
			bApplyToTestSpeed = ResultAngle >= MaxSlopeAngle;
			//if (!bApplyToTestSpeed)
			//{
			//	if (ResultAngle >= MaxSlopeAngle)
			//	{
			//		bApplyToTestSpeed = true;
			//	}
			//}

			const float SlantedWallClamp = 80.f;
			if (ResultAngle > SlantedWallClamp)
			{
				HitNormal.Z = 0;
				HitNormal.Normalize();
			}

			//if (!DidTouchGround && ResultAngle < MaxSlopeAngle)
			//{
			//	DidTouchGround = true;
			//	//if (TestVelocity.Z <= 0)
			//	//	Translation.Z = 0;
			//	//else
			//	//	Translation.Z = TestVelocity.Z;
			//}

			Translation = FVector::VectorPlaneProject(Translation, HitNormal);//SpeedApplyTranslation

			// Project along the ground.
			Owner->AddActorWorldOffset(Translation * DeltaTime, true, &Hit);
			// Exit prematurely if we didn't collide with anything this sweep.
			if (!Hit.bBlockingHit)
				break;

			IterationLimit--;
		}

		if (IterationLimit == 0)
			Translation = FVector::ZeroVector;
	}
#pragma endregion 
	if (bApplyToTestSpeed)
	TestVelocity = FVector(Translation.X, Translation.Y, TestVelocity.Z);

	TestOnGround = DidTouchGround;
}
