#include "AstroShoppingUserSettings.h"

UAstroShoppingUserSettings* UAstroShoppingUserSettings::GetAstroShoppingUserSettings()
{
	return Cast<UAstroShoppingUserSettings>(UGameUserSettings::GetGameUserSettings());
}

FString UAstroShoppingUserSettings::GetProxyApiKey()
{
	return ProxyApiKey;
}

void UAstroShoppingUserSettings::SetProxyApiKey(const FString& Value)
{
	if (!ProxyApiKey.Equals(Value))
	{
		ProxyApiKey = Value;
		OnProxyApiKeyChanged.Broadcast(Value);
		OnProxyApiKeyChangedNative.Broadcast(Value);
	}
}

FString UAstroShoppingUserSettings::GetGenieRefreshToken()
{
	return GenieRefreshToken;
}

void UAstroShoppingUserSettings::SetGenieRefreshToken(const FString& Value)
{
	if (!GenieRefreshToken.Equals(Value))
	{
		GenieRefreshToken = Value;
		OnGenieRefreshTokenChanged.Broadcast(Value);
		OnGenieRefreshTokenChangedNative.Broadcast(Value);
	}
}

float UAstroShoppingUserSettings::GetMasterVolume()
{
	return MasterVolume;
}

void UAstroShoppingUserSettings::SetMasterVolume(float Value) 
{
	MasterVolume = Value;
}

float UAstroShoppingUserSettings::GetMusicVolume()
{
	return MusicVolume;
}

void UAstroShoppingUserSettings::SetMusicVolume(float Value)
{
	MusicVolume = Value;
}

float UAstroShoppingUserSettings::GetMouseSensitivity()
{
	return MouseSensitivity;
}

void UAstroShoppingUserSettings::SetMouseSensitivity(float Value)
{
	if (!FMath::IsNearlyEqual(MouseSensitivity, Value))
	{
		MouseSensitivity = Value;
		OnMouseSensitivityChanged.Broadcast(Value);
		OnMouseSensitivityChangedNative.Broadcast(Value);
	}
}