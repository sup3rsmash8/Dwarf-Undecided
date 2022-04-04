// (C) 2021 Changemaker Educations (FG20FT GP3 Team 8) - All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Newspaper/ArticleData.h"
#include "NewsArticle_DataAsset.generated.h"

/**
 * 
 */
UCLASS()
class COOLGP3PROJECT_API UNewsArticle_DataAsset : public UDataAsset
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere)
	FArticleData ArticleData;

public:
	/* Returns this asset's article data. */
	UFUNCTION(BlueprintCallable)
	FArticleData GetArticleData() const;
};
