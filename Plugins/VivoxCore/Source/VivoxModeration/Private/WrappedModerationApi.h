 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once
#include "CoreMinimal.h"
#include "IVivoxModerationApi.h"
#include "OpenAPIReportApi.h"

namespace VivoxModeration
{
	/**
	 * @brief Wrapper class for the generated OpenAPI code.
	 */
	class FWrappedModerationApi : public IVivoxModerationApi
	{

	public:

		FWrappedModerationApi();
		virtual ~FWrappedModerationApi() override;
		virtual FHttpRequestPtr ReportPost(const OpenAPIReportApi::ReportPostRequest& Request, const OpenAPIReportApi::FReportPostDelegate& Delegate) override;
		virtual void AddHeaderParam(const FString& Key, const FString& Value) override;
		virtual void ClearHeaderParams() override;

	private:

		TUniquePtr<OpenAPIReportApi> Api;

	};

}