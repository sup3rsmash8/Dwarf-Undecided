// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

// AUTHOR: Justus Hörberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoldCarryWidget.generated.h"

UENUM()
enum class EGoldSetType
{
	/* Gold value will be set immediately. */
	Instant = 0,
	/* Gold will increase at a constant rate relative to the change. */
	FastConstant = 1,
	/* Gold will increase quickly, then slowly, like an exponential curve. */
	FastToSlow = 2
};

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UGoldCarryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GoldTextBlock;
	
	/* Is the widget currently approaching the current gold value? */
	bool bIsCurrentlyApproachingGoldValue;
	/* The value being represented. Is a floating point to make interpolation smoother, gets converted into an int when applied to the text widget. */
	UPROPERTY(BlueprintReadOnly)
		float RepresentedGoldValue;

	/* The way the widget gold value interpolates towards new gold values - should it be instant, smooth or constant? */
	//UPROPERTY(EditAnywhere, Category = "GoldWidgetSettings")
	//	EGoldSetType GoldInterpolationType;

	UPROPERTY(EditAnywhere, Category = "GoldWidgetSettings")
		float InterpolationDuration;

	/* Is called whenever RepresentedGoldValue reaches a new integer. This is typically where the widget gets updated. */
	virtual void OnNewGoldValueSet(float Value);

	int PrePurchaseGoldValue;
	int TargetGoldValue;

private:
	UFUNCTION()
		void OnGoldChanged(int OldGoldValue, int NewGoldValue);

public:
	/* Returns an integer value in the format 00000000. */
	UFUNCTION(BlueprintCallable)
		static FText GetNumberInGoldFormat(int Value);

	static float InterpByType(float CurrentValue, float PreviousValue, float TargetValue, float Duration, float DeltaTime/*, EGoldSetType InterpType*/);
};
