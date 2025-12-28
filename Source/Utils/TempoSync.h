#pragma once

#include <juce_core/juce_core.h>
#include <array>

namespace Chronos {

enum class SyncDivision
{
    _1_64,      // 1/64
    _1_32,      // 1/32
    _1_16T,     // 1/16 triplet
    _1_16,      // 1/16
    _1_16D,     // 1/16 dotted
    _1_8T,      // 1/8 triplet
    _1_8,       // 1/8
    _1_8D,      // 1/8 dotted
    _1_4T,      // 1/4 triplet
    _1_4,       // 1/4
    _1_4D,      // 1/4 dotted
    _1_2T,      // 1/2 triplet
    _1_2,       // 1/2
    _1_2D,      // 1/2 dotted
    _1_1,       // 1 bar
    _2_1,       // 2 bars
    NumDivisions
};

struct DivisionInfo
{
    const char* name;
    float multiplier;  // Multiplier of quarter note duration
};

// Multipliers relative to quarter note
inline const std::array<DivisionInfo, static_cast<size_t>(SyncDivision::NumDivisions)> divisionTable = {{
    {"1/64",      0.0625f},
    {"1/32",      0.125f},
    {"1/16T",     0.1667f},   // 1/6
    {"1/16",      0.25f},
    {"1/16D",     0.375f},    // 1/16 * 1.5
    {"1/8T",      0.3333f},   // 1/3
    {"1/8",       0.5f},
    {"1/8D",      0.75f},     // 1/8 * 1.5
    {"1/4T",      0.6667f},   // 2/3
    {"1/4",       1.0f},
    {"1/4D",      1.5f},      // 1/4 * 1.5
    {"1/2T",      1.3333f},   // 4/3
    {"1/2",       2.0f},
    {"1/2D",      3.0f},      // 1/2 * 1.5
    {"1/1",       4.0f},
    {"2/1",       8.0f}
}};

inline float getDivisionMultiplier(SyncDivision div)
{
    return divisionTable[static_cast<size_t>(div)].multiplier;
}

inline const char* getDivisionName(SyncDivision div)
{
    return divisionTable[static_cast<size_t>(div)].name;
}

// Calculate delay time in ms from BPM and division
inline float calculateDelayMs(float bpm, SyncDivision division)
{
    if (bpm <= 0.0f)
        bpm = 120.0f;  // Default

    // Quarter note duration in ms = 60000 / BPM
    float quarterNoteMs = 60000.0f / bpm;

    // Apply division multiplier
    return quarterNoteMs * getDivisionMultiplier(division);
}

// Calculate LFO rate in Hz from BPM and division
inline float calculateLFORateHz(float bpm, SyncDivision division)
{
    float periodMs = calculateDelayMs(bpm, division);
    return 1000.0f / periodMs;  // Convert period to frequency
}

// Get string array for combo box
inline juce::StringArray getDivisionNames()
{
    juce::StringArray names;
    for (const auto& div : divisionTable)
        names.add(div.name);
    return names;
}

} // namespace Chronos
