 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#include "IVivoxModerationApi.h"
#include "Utils/ModerationUtils.h"
#include "Models/VivoxModerationReport.h"
#include "Models/VivoxModerationReportResponse.h"

namespace VivoxModeration
{

#if ENGINE_MAJOR_VERSION == 5
	class FVivoxModerationApi : public FTSTickerObjectBase
#else
	class FVivoxModerationApi : public FTickerObjectBase
#endif
	{

	public:

		/**
		 * @brief Instantiates a new API wrapper without a project Id or environment Id.
		 */
		FVivoxModerationApi();

		/**
		 * @brief                Instantiates a new API wrapper with a unique API wrapper instance pointer.
		 * @param Api            The API wrapper instance pointer.
		 */
		FVivoxModerationApi(TUniquePtr<IVivoxModerationApi> Api);

		/**
		 * @brief           Function overload from FTickerObjectBase/FTSTickerObjectBase.
		 * @param DeltaTime Time (in seconds) between each tick.
		 */
		bool Tick(float DeltaTime) final;

		/**
		 * @brief                 Signs in the current user anonymously. No credentials are required and the session is confined to the current device.
		 * @param Report          A constructed Report object with the Reported User's Unity Authentication PlayerId and the Report Reason
		 * @param ResponseHandler Handler that executes when the API has returned a response.
		 */
		void ReportPost(FVivoxModerationReport Report, FString UASToken, VivoxModeration::THandler<FVivoxModerationReportResponse> ResponseHandler = VivoxModeration::THandler<FVivoxModerationReportResponse>());

		/**
		 * @brief Resets variables for moderation operations.
		 */
		void Flush();

	private:

		/*!< A pointer to the report post request object. */
		FHttpRequestPtr ReportPostRequestPtr;
		/*!< Handler that executes when a report response has been received. */
		VivoxModeration::THandler<FVivoxModerationReportResponse> ReportDelegate;

		TUniquePtr<VivoxModeration::IVivoxModerationApi> Api;

		void HandleErrorResponse(TSharedPtr<FJsonValue> JsonParsed);

		void OnReportPost(const OpenAPIReportApi::ReportPostResponse& ReportResponse);
	};

}