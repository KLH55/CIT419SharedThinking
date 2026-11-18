 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once
#include "CoreMinimal.h"
#include "OpenAPIReportApi.h"
#include "OpenAPIReportApiOperations.h"

namespace VivoxModeration
{

	class IVivoxModerationApi
	{

	public:

		virtual ~IVivoxModerationApi() {};
		virtual FHttpRequestPtr ReportPost(const OpenAPIReportApi::ReportPostRequest &Request, const OpenAPIReportApi::FReportPostDelegate&Delegate) = 0;
		virtual void AddHeaderParam(const FString& Key, const FString& Value) = 0;
		virtual void ClearHeaderParams() = 0;

	};

}