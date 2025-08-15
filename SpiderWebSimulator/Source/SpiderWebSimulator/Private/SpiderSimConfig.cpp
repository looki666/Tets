#include "SpiderSimConfig.h"

USpiderSimConfig::USpiderSimConfig()
{
    // Default Physics values
    Gravity.Value = 9.8f;
    Gravity.Multiplier = 10.0f; // Unreal units are cm, so gravity is ~980

    WindStrength.Value = 1.0f;
    WindStrength.bIsRandomized = true;
    WindStrength.RandomFrom = 0.5f;
    WindStrength.RandomTo = 2.0f;
    WindStrength.Multiplier = 50.0f;

    // Default AI values
    SpiderSpeed.Value = 150.0f; // cm/s

    // Default Web values
    WebBreakThreshold.Value = 2.0f;
    WebBreakThreshold.Multiplier = 1000.0f;

    // Default GOAP values
    GoalChangeFrequency = 5.0f; // seconds
}
