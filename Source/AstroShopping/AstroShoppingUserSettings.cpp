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
	ProxyApiKey = Value;
}

FString UAstroShoppingUserSettings::GetGenieRefreshToken()
{
	return GenieRefreshToken;
}

void UAstroShoppingUserSettings::SetGenieRefreshToken(const FString& Value)
{
	GenieRefreshToken = Value;
}
