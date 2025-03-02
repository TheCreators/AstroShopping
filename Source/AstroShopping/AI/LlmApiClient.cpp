#include "LlmApiClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FLlmApiClient::FLlmApiClient(const FString& InApiKey, const FString& InBaseUrl) : ApiKey(InApiKey), BaseUrl(InBaseUrl)
{
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
            "contents": [{
                "role": "user",
                "parts": [{"text": "%s"}]
            }]
        })"),
        *SanitizedText
    );
}

FString FLlmApiClient::ExtractTextFromJson(const FString& JsonString) const
{
    TSharedPtr<FJsonObject> JsonResponseObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    FString ExtractedText;

    if (FJsonSerializer::Deserialize(Reader, JsonResponseObject) && JsonResponseObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* CandidatesArray;
        if (JsonResponseObject->TryGetArrayField(TEXT("candidates"), CandidatesArray) && CandidatesArray->Num() > 0)
        {
            const TSharedPtr<FJsonObject>* CandidateObject;
            if ((*CandidatesArray)[0]->TryGetObject(CandidateObject))
            {
                const TSharedPtr<FJsonObject>* ContentObject;
                if ((*CandidateObject)->TryGetObjectField(TEXT("content"), ContentObject))
                {
                    const TArray<TSharedPtr<FJsonValue>>* PartsArray;
                    if ((*ContentObject)->TryGetArrayField(TEXT("parts"), PartsArray) && PartsArray->Num() > 0)
                    {
                        const TSharedPtr<FJsonObject>* PartObject;
                        if ((*PartsArray)[0]->TryGetObject(PartObject))
                        {
                            (*PartObject)->TryGetStringField(TEXT("text"), ExtractedText);
                        }
                    }
                }
            }
        }
    }

    return ExtractedText;
}