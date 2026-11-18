 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpResponse.h"
#include "VivoxModerationReportResponse.generated.h"



/**
* @brief Struct representing a moderation report response.
*/
USTRUCT(BlueprintType)
struct VIVOXMODERATION_API FVivoxModerationReportResponse
{
	GENERATED_BODY()

	/*!< Whether the response was a success. */
	UPROPERTY(BlueprintReadOnly, Category = "Unity Gaming Services | Moderation")
	bool bWasSuccessful = false;
};