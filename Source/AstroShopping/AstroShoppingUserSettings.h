#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "AstroShoppingUserSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseSensitivityChanged, float, NewSensitivity);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseSensitivityChangedNative, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProxyApiKeyChanged, FString, NewProxyApiKey);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseProxyApiKeyChangedNative, FString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGenieRefreshTokenChanged, FString, NewGenieRefreshToken);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGenieRefreshTokenChangedNative, FString);

UCLASS()
class ASTROSHOPPING_API UAstroShoppingUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UAstroShoppingUserSettings* GetAstroShoppingUserSettings();

	UFUNCTION(BlueprintCallable)
	FString GetProxyApiKey();

	UFUNCTION(BlueprintCallable)
	void SetProxyApiKey(const FString& Value);

	UPROPERTY(BlueprintAssignable)
	FOnProxyApiKeyChanged OnProxyApiKeyChanged;

	FOnMouseProxyApiKeyChangedNative OnProxyApiKeyChangedNative;

	UFUNCTION(BlueprintCallable)
	FString GetGenieRefreshToken();

	UFUNCTION(BlueprintCallable)
	void SetGenieRefreshToken(const FString& Value);

	UPROPERTY(BlueprintAssignable)
	FOnGenieRefreshTokenChanged OnGenieRefreshTokenChanged;

	FOnGenieRefreshTokenChangedNative OnGenieRefreshTokenChangedNative;

	UFUNCTION(BlueprintCallable)
	float GetMasterVolume();

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float Value);

	UFUNCTION(BlueprintCallable)
	float GetMusicVolume();

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float Value);

	UFUNCTION(BlueprintCallable)
	float GetMouseSensitivity();

	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float Value);

	UPROPERTY(BlueprintAssignable)
	FOnMouseSensitivityChanged OnMouseSensitivityChanged;

	FOnMouseSensitivityChangedNative OnMouseSensitivityChangedNative;
	
protected:
	UPROPERTY(Config)
	FString ProxyApiKey;

	UPROPERTY(Config)
	FString GenieRefreshToken;

	UPROPERTY(Config)
	float MasterVolume = 0.5;

	UPROPERTY(Config)
	float MusicVolume = 0.5;

	UPROPERTY(Config)
	float MouseSensitivity = 1;
};
