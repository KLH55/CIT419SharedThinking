 /*
 * #####################################################################################
 *  Unity Vivox Plugin for Unreal Engine Copyright © 2019 Unity Technologies
 * #####################################################################################
 */

#pragma once
#include "CoreMinimal.h"

/**
 * @brief Overall reasons accepted for a Moderation Report.
 */
enum class EVivoxModerationReportReason
{
	AimSnapping,
	Boosting,
	Exploiting,
	Hacking,
	Smurfing,
	UnrealisticMovement,
	CollusionWithOpponent,
	LeftTheMatch,
	Inactive,
	Sabotage,
	Spamming,
	HateSpeech,
	PredatoryBehaviour,
	NoiseDisruption,
	Scamming,
	Ads,
	Threat,
	VerbalAbuse,
	PlayerName,
};