#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "AstroShoppingUserSettings.generated.h"

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

	UFUNCTION(BlueprintCallable)
	FString GetGenieRefreshToken();

	UFUNCTION(BlueprintCallable)
	void SetGenieRefreshToken(const FString& Value);

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
	
protected:
	UPROPERTY(Config)
	FString ProxyApiKey;

	UPROPERTY(Config)
	FString GenieRefreshToken;

	UPROPERTY(Config)
	float MasterVolume;

	UPROPERTY(Config)
	float MusicVolume;

	UPROPERTY(Config)
	float MouseSensitivity;
};
