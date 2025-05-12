#include "LlmApiClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FLlmApiClient::FLlmApiClient(const FString& InApiKey, const FString& InBaseUrl, const FString& InModel)
	: ApiKey(InApiKey)
	, BaseUrl(InBaseUrl)
	, Model(InModel)
{
}

void FLlmApiClient::SetApiKey(const FString& NewApiKey)
{
    ApiKey = NewApiKey;
}

void FLlmApiClient::SendRequest(const FString& InputText,
    TFunction<void(FString)> OnSuccess,
    TFunction<void(FString)> OnError,
    bool bReturnOnlyText)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
	UE_LOG(LogTemp, Warning, TEXT("Request URL: %s"), *BaseUrl);
	UE_LOG(LogTemp, Warning, TEXT("Request Body: %s"), *CreateRequestBody(InputText));
    Request->SetContentAsString(CreateRequestBody(InputText));

    Request->OnProcessRequestComplete().BindLambda([this, OnSuccess, OnError, bReturnOnlyText](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
        {
            if (bSuccess && Response.IsValid())
            {
                if (Response->GetResponseCode() == 200)
                {
                    FString ResponseContent = Response->GetContentAsString();
                    if (bReturnOnlyText)
                    {
                        OnSuccess(ExtractTextFromJson(ResponseContent));
                    }
                    else
                    {
                        OnSuccess(ResponseContent);
                    }
                }
                else
                {
                    OnError(FString::Printf(TEXT("API Error: %d - %s"),
                        Response->GetResponseCode(),
                        *Response->GetContentAsString()));
                }
            }
            else
            {
                OnError(TEXT("Request failed or response is invalid."));
            }
        });

    Request->ProcessRequest();
}

FString FLlmApiClient::CreateRequestBody(const FString& InputText) const
{
    FString SanitizedText = InputText.Replace(TEXT("\""), TEXT("\\\""));

    return FString::Printf(
        TEXT(R"({
            "model": "%s",
            "input": "%s"
        })"),
		*Model,
        *SanitizedText
    );
}

FString FLlmApiClient::ExtractTextFromJson(const FString& JsonString) const
{
    // Parse the raw JSON string
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    FString ExtractedText;

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // 1) Get the "output" array
        const TArray<TSharedPtr<FJsonValue>>* OutputArray;
        if (JsonObject->TryGetArrayField(TEXT("output"), OutputArray) && OutputArray->Num() > 0)
        {
            // 2) Take the first element of "output"
            const TSharedPtr<FJsonValue>& OutputValue = (*OutputArray)[0];
            const TSharedPtr<FJsonObject>* OutputObj;
            if (OutputValue->TryGetObject(OutputObj))
            {
                // 3) Get the "content" array inside that object
                const TArray<TSharedPtr<FJsonValue>>* ContentArray;
                if ((*OutputObj)->TryGetArrayField(TEXT("content"), ContentArray) && ContentArray->Num() > 0)
                {
                    // 4) Take the first element of "content"
                    const TSharedPtr<FJsonValue>& ContentValue = (*ContentArray)[0];
                    const TSharedPtr<FJsonObject>* ContentObj;
                    if (ContentValue->TryGetObject(ContentObj))
                    {
                        // 5) Read the "text" field
                        (*ContentObj)->TryGetStringField(TEXT("text"), ExtractedText);
                    }
                }
            }
        }
    }

    return ExtractedText;
}