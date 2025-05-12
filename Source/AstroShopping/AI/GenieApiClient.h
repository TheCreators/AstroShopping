#pragma once

#include "CoreMinimal.h"
#include "GenieApiClient.generated.h"

USTRUCT()
struct FModelStatusResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString Status;

	UPROPERTY()
	FString ModelUrl;

	UPROPERTY()
	FString ThumbnailUrl;
};

USTRUCT()
struct FModelsGenerationResponse
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> ModelIds;
};

class ASTROSHOPPING_API FGenieApiClient
{
public:
	FGenieApiClient(const FString& InRefreshToken);

	void SendModelsGenerationRequest(const FString& Prompt,
		TFunction<void(const FModelsGenerationResponse&)> OnSuccess,
		TFunction<void(FString)> OnError);

	void SendModelsStatusRequest(const FString& Id,
		TFunction<void(const FModelStatusResponse&)> OnSuccess,
		TFunction<void(FString)> OnError);

	void SetRefreshToken(const FString& NewRefreshToken);

private:
	FString CreateRequestBody(const FString& Prompt) const;

	FModelsGenerationResponse GetModelsGenerationResponse(const FString& JsonString) const;

	FModelStatusResponse GetModelStatusResponse(const FString& JsonString) const;

	FString RefreshToken;

	FString BaseUrl;
};
