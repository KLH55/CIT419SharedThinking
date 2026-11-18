 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#include "WrappedModerationApi.h"
#include "OpenAPIReportApi.h"
#include "OpenAPIReportApiOperations.h"

namespace VivoxModeration
{

	FWrappedModerationApi::FWrappedModerationApi()
	{
		Api = MakeUnique<OpenAPIReportApi>();
#ifdef WITH_STAGING
		Api->SetURL("https://moderation-event-stg.services.api.unity.com/v1alpha1");
#else
		Api->SetURL("https://moderation-event.services.api.unity.com/v1alpha1");
#endif
	}

	FWrappedModerationApi::~FWrappedModerationApi()
	{

	}

	FHttpRequestPtr FWrappedModerationApi::ReportPost(const OpenAPIReportApi::ReportPostRequest& Request, const OpenAPIReportApi::FReportPostDelegate& Delegate)
	{
		return Api->ReportPost(Request, Delegate);
	}

	void FWrappedModerationApi::AddHeaderParam(const FString& Key, const FString& Value)
	{
		Api->AddHeaderParam(Key, Value);
	}

	void FWrappedModerationApi::ClearHeaderParams()
	{
		Api->ClearHeaderParams();
	}

}