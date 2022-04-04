// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.


#include "UI/GoldCarryWidget.h"

#include "Components/TextBlock.h"
#include "GameSystem/GP3GameMode.h"

void UGoldCarryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AGP3GameMode* GMode = GetWorld()->GetAuthGameMode<AGP3GameMode>();

	// Try adding gold delegate.
	if (GMode)
	{
		GMode->OnGoldChangedDelegate.AddDynamic(this, &UGoldCarryWidget::OnGoldChanged);
	}

	GoldTextBlock->SetText(FText::FromString(TEXT("00000000")));

	//
}

void UGoldCarryWidget::NativeDestruct()
{
	AGP3GameMode* GMode = GetWorld()->GetAuthGameMode<AGP3GameMode>();

	// Try deleting gold delegate.
	if (GMode)
	{
		GMode->OnGoldChangedDelegate.RemoveDynamic(this, &UGoldCarryWidget::OnGoldChanged);
	}

	Super::NativeDestruct();
}

void UGoldCarryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsCurrentlyApproachingGoldValue)
	{
		
		RepresentedGoldValue = InterpByType(RepresentedGoldValue, PrePurchaseGoldValue, TargetGoldValue, InterpolationDuration, InDeltaTime/*, GoldInterpolationType*/);
		
		
		int FlooredToInt = FMath::RoundToInt(RepresentedGoldValue);
		
		OnNewGoldValueSet(FlooredToInt);
		
		if (FMath::Abs(FlooredToInt) >= FMath::Abs(TargetGoldValue))
		{
			RepresentedGoldValue = TargetGoldValue;
			
			FlooredToInt = FMath::RoundToInt(RepresentedGoldValue);
			OnNewGoldValueSet(FlooredToInt);
			
			bIsCurrentlyApproachingGoldValue = false;
		}
		
		
	}
}

void UGoldCarryWidget::OnGoldChanged(int OldGoldValue, int NewGoldValue)
{
	TargetGoldValue = NewGoldValue;
	
	if (!bIsCurrentlyApproachingGoldValue)
	{
		PrePurchaseGoldValue = OldGoldValue;
		bIsCurrentlyApproachingGoldValue = true;
	}
}

FText UGoldCarryWidget::GetNumberInGoldFormat(int Value)
{
	const FString CharFormat(TEXT("00000000"));
	const FString ChZero = TEXT("0");

	// Loop back around if exceeding the max allowed by the format.
	Value = Value % (int)FMath::Pow(10, CharFormat.Len());

	FString NumToStr = FString::FromInt(Value);
	
	
	
	FString FullStr;
	// Build the string. Fill irrelevant digits with zeroes first.
	for (int Index = 0; Index < CharFormat.Len(); Index++)
	{
		// If NumIndex returns a negative number, keep adding zeroes.
		const int NumIndex = NumToStr.Len() - (CharFormat.Len()) + Index;
		
		//UE_LOG(LogTemp, Log, TEXT("%i"), NumToStr.Len());
		
		// Once NumIndex reaches an indexable number, just fill the rest of the string with NumToStr.
		if (NumIndex < 0)
			FullStr += ChZero;
		else if (NumIndex < NumToStr.Len())
		{
			FullStr += NumToStr;
			break;
		}

	}
	
	return FText::FromString(FullStr);
}

void UGoldCarryWidget::OnNewGoldValueSet(float Value)
{
	FText NewTextValue = GetNumberInGoldFormat(FMath::FloorToInt(Value));
	//FString E = NewTextValue.ToString();
	
	GoldTextBlock->SetText(NewTextValue);
}

float UGoldCarryWidget::InterpByType(float CurrentValue, float PreviousValue, float TargetValue, float Duration, float DeltaTime/*, EGoldSetType InterpType*/)
{
	// Prevent division by zero, so we might as well make it "instant"
	if (/*InterpType == EGoldSetType::Instant || */Duration == 0)
		return TargetValue;

	float Delta = /*FMath::Abs(*/TargetValue - PreviousValue/*)*/;
	float IntrpSpeed = (float)Delta / Duration;
	float OutValue = CurrentValue + (IntrpSpeed * DeltaTime);//FMath::FInterpConstantTo(CurrentValue, TargetValue, DeltaTime, IntrpSpeed);
	return OutValue;
}