#include "WebBlueprintFunctionLibrary.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"

void UWebBlueprintFunctionLibrary::ClearWebBrowserCache()
{
    IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
    if (WebBrowserSingleton)
    {
        TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
        if (CookieManager.IsValid())
        {
            CookieManager->DeleteCookies();
        }
    }
}
