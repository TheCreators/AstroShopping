#include "GenieApiClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FGenieApiClient::FGenieApiClient(const FString& InRefreshToken) : RefreshToken(InRefreshToken)
{
	BaseUrl = TEXT("https://webapp.engineeringlumalabs.com/api/v3");
}

void FGenieApiClient::SendModelsGenerationRequest(const FString& Prompt,
	TFunction<void(const FModelsGenerationResponse&)> OnSuccess,
	TFunction<void(FString)> OnError)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/creations"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *RefreshToken));
	Request->SetContentAsString(CreateRequestBody(Prompt));

    Request->OnProcessRequestComplete().BindLambda([this, OnSuccess, OnError](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
        {
            if (bSuccess && Response.IsValid())
            {
                if (Response->GetResponseCode() == 200)
                {
                    OnSuccess(GetModelsGenerationResponse(Response->GetContentAsString()));
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

void FGenieApiClient::SendModelsStatusRequest(const FString& Id, TFunction<void(const FModelStatusResponse&)> OnSuccess, TFunction<void(FString)> OnError)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/creations/uuid/") + Id);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([this, OnSuccess, OnError](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
        {
            if (bSuccess && Response.IsValid())
            {
                if (Response->GetResponseCode() == 200)
                {
					OnSuccess(GetModelStatusResponse(Response->GetContentAsString()));
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

FString FGenieApiClient::CreateRequestBody(const FString& Prompt) const
{
    FString SanitizedText = Prompt.Replace(TEXT("\""), TEXT("\\\""));

    return FString::Printf(
        TEXT(R"({
            "input": {
                "text": "%s",
                "type": "imagine_3d_one",
                "jobParams":{"seed":"%d"}
            },
            "client":"web"
        })"),
        *SanitizedText,
		FMath::Rand()
    );
}

FModelsGenerationResponse FGenieApiClient::GetModelsGenerationResponse(const FString& JsonString) const
{
    TSharedPtr<FJsonObject> JsonResponseObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TArray<FString> ResultIds;

    if (FJsonSerializer::Deserialize(Reader, JsonResponseObject) && JsonResponseObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>> ResponseArray = JsonResponseObject->GetArrayField(TEXT("response"));

        for (const TSharedPtr<FJsonValue>& Item : ResponseArray)
        {
            if (Item->Type == EJson::String)
            {
                ResultIds.Add(Item->AsString());
            }
        }
    }

	return { ResultIds };
}

FModelStatusResponse FGenieApiClient::GetModelStatusResponse(const FString& JsonString) const
{
	FModelStatusResponse Response = { TEXT(""), TEXT(""), TEXT("") };
    TSharedPtr<FJsonObject> JsonResponseObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonResponseObject) && JsonResponseObject.IsValid())
    {
        const TSharedPtr<FJsonObject> ResponseObject = JsonResponseObject->GetObjectField(TEXT("response"));

        Response.Status = ResponseObject->GetStringField(TEXT("status"));

        if (!ResponseObject->HasField(TEXT("output")))
		{
			return Response;
		}

        const TArray<TSharedPtr<FJsonValue>> OutputArray = ResponseObject->GetArrayField(TEXT("output"));

        for (const TSharedPtr<FJsonValue>& Item : OutputArray)
        {
            const TSharedPtr<FJsonObject> OutputItem = Item->AsObject();
            const TSharedPtr<FJsonObject> Metadata = OutputItem->GetObjectField(TEXT("metadata"));

            if (Metadata->GetStringField(TEXT("key")) == "model_glb")
            {
                Response.ModelUrl = OutputItem->GetStringField(TEXT("file_url"));
            }
            else if (Metadata->GetStringField(TEXT("key")) == "thumbnail")
            {
                Response.ThumbnailUrl = OutputItem->GetStringField(TEXT("file_url"));
            }
        }
    }

    return Response;
}

