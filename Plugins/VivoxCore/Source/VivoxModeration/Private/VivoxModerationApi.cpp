 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#include "VivoxModerationApi.h"
#include "WrappedModerationApi.h"
#include "VivoxModerationModule.h"
#include "OpenAPIReportApiOperations.h"
#include "OpenAPIErrorsErrorResponseBody.h"
#include "OpenAPIModelPlayersRequest.h"
#include "OpenAPIModelVivoxChannelRequest.h"
#include "OpenAPIReportApi.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

namespace VivoxModeration
{
	FVivoxModerationApi::FVivoxModerationApi()
	{
		Api = MakeUnique<FWrappedModerationApi>();
	}

	FVivoxModerationApi::FVivoxModerationApi(TUniquePtr<IVivoxModerationApi> Api)
		: Api(MoveTemp(Api))
	{

	}

	bool FVivoxModerationApi::Tick(float DeltaTime)
	{
		return true;
	}

	void FVivoxModerationApi::ReportPost(FVivoxModerationReport Report, FString UASToken, VivoxModeration::THandler<FVivoxModerationReportResponse> ResponseHandler)
	{
		//Reset variables
		Flush();
		OpenAPIModelPostReportRequest RequestPayload;

		if (Report.VivoxChannels.Num() != 0)
		{
			TMap<AccountId, TArray<FString>> cachedPlayers;
			TArray<VivoxModeration::OpenAPIModelVivoxChannelRequest> channelArray;
			TArray<VivoxModeration::OpenAPIModelPlayersRequest> playerArray;
			for (auto cacheChannel : Report.VivoxChannels)
			{
				OpenAPIModelVivoxChannelRequest ChannelRequest;
				ChannelRequest.Name = cacheChannel.Key.Name();
				ChannelRequest.Uri = cacheChannel.Key.ToString();
				channelArray.Add(ChannelRequest);
				for (auto cachePlayer : cacheChannel.Value)
				{
					if (cachedPlayers.Contains(cachePlayer))
					{
						cachedPlayers[cachePlayer].Add(cacheChannel.Key.Name());
						continue;
					}
					TArray<FString> cacheChannelArray;
					cacheChannelArray.Add(cacheChannel.Key.ToString());
					cachedPlayers.Add(cachePlayer, cacheChannelArray);
				}
			}
			for (auto cachedPlayer : cachedPlayers)
			{
				OpenAPIModelPlayersRequest PlayersRequest;
				PlayersRequest.PlayerId = cachedPlayer.Key.Name();
				PlayersRequest.VivoxId = cachedPlayer.Key.ToString();
				PlayersRequest.Channels = cachedPlayer.Value;
				playerArray.Add(PlayersRequest);
			}
			RequestPayload.VivoxChannels = channelArray;
			RequestPayload.Players = playerArray;
		}

		//Create sign-in request
		OpenAPIReportApi::ReportPostRequest ReportRequest;
		ReportDelegate = ResponseHandler;

		RequestPayload.ReportReason = Report.ReportReason;

		RequestPayload.ReportedUnityPlayerId = Report.ReportedUnityPlayerID;
		RequestPayload.ReportTimestamp = FDateTime::Now().ToIso8601();
		RequestPayload.ReportedVivoxPlayerId = Report.ReportedVivoxURI;
		RequestPayload.ReportingVivoxPlayerId = Report.ReportingVivoxURI;

		//Append request body
		ReportRequest.Payload = RequestPayload;

		//Bind delegate
		OpenAPIReportApi::FReportPostDelegate Delegate;
		Delegate.BindRaw(this, &FVivoxModerationApi::OnReportPost);

		Api->ClearHeaderParams();

		Api->AddHeaderParam("authorization", "Bearer " + UASToken);
		Api->AddHeaderParam("Accept", "application/json");

		//Send request
		UE_LOG(LogModeration, Verbose, TEXT("Attempting to post Moderation report"));
		ReportPostRequestPtr = Api->ReportPost(ReportRequest, Delegate);
	}

	void FVivoxModerationApi::OnReportPost(const OpenAPIReportApi::ReportPostResponse& ReportResponse)
	{
		//Response parsing
		const FHttpResponsePtr& HttpResponse = ReportResponse.GetHttpResponse(); //Raw HTTP response body
		int32 ResponseCode = HttpResponse->GetResponseCode(); //HTTP response code
		FString ResponseBody = HttpResponse->GetContentAsString(); //Response body as JSON string

		//Response struct
		FVivoxModerationReportResponse ResponseStruct;
		ResponseStruct.bWasSuccessful = false;

		//JSON Parsing utilities
		TSharedPtr<FJsonValue> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseBody);

		if (ReportResponse.IsSuccessful())
		{
			ResponseStruct.bWasSuccessful = true;
			ReportDelegate.ExecuteIfBound(ResponseStruct);
			ReportDelegate.Unbind();

			return;
		}

		//Deserialize failure response JSON
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			HandleErrorResponse(JsonParsed);
		}

		ReportDelegate.ExecuteIfBound(ResponseStruct);
		ReportDelegate.Unbind();
	}

	void FVivoxModerationApi::HandleErrorResponse(TSharedPtr<FJsonValue> JsonParsed)
	{
		//Deserialize failure response JSON
		if (JsonParsed.IsValid())
		{
			TUniquePtr<OpenAPIErrorsErrorResponseBody> ErrorResponseBody = MakeUnique<OpenAPIErrorsErrorResponseBody>();

			if (ErrorResponseBody->FromJson(JsonParsed))
			{
				//Get data from failure response
				int32 Status = ErrorResponseBody->Status.Get(-1);
				FString Detail = ErrorResponseBody->Detail.Get(TEXT(""));
				FString Title = ErrorResponseBody->Title.Get(TEXT(""));

				UE_LOG(LogModeration, Error, TEXT("Unity Moderation responded with a failure! %d: %s (%s)"), Status, *Title, *Detail);
				return;
			}
			else
			{
				//Parsing error
				UE_LOG(LogModeration, Error, TEXT("Failed to parse moderation JSON failure response body!"));
				return;
			}
		}

		//Deserialization error
		UE_LOG(LogModeration, Error, TEXT("Failed to deserialize moderation JSON failure response body!"));
	}

	void FVivoxModerationApi::Flush()
	{
		//Unbind delegates
		ReportDelegate.Unbind();
	}

}