#pragma once

#include "CoreMinimal.h"

class ASTROSHOPPING_API FLlmApiClient
{
public:
	FLlmApiClient(const FString& InApiKey, const FString& InBaseUrl, const FString& InModel);
	
	void SendRequest(const FString& InputText,
		TFunction<void(FString)> OnSuccess,
		TFunction<void(FString)> OnError,
		bool bFullReturn = true);

private:
	FString ApiKey;

	FString BaseUrl;

	FString Model;

	FString CreateRequestBody(const FString& InputText) const;
	
	FString ExtractTextFromJson(const FString& JsonString) const;
};
