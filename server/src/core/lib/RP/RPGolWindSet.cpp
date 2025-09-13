#include <cstdlib>
#include <cmath>
#include <vector>
#include <functional>

#include "RPGolWindSet.h"
#include "RPGolDefine.h"
#include "RPGolConfig.h"

std::vector<unsigned int> RPGolWindSet::hashesWithDepth(int depth) const
{
    std::vector<unsigned int> hashes;

    // recursive function to generate hashes
    std::function<void(int, unsigned int)> generateHashes = [&](int index, unsigned int hash)
    {
        if (index == depth)
        {
            hashes.push_back(hash);
            return;
        }

        std::vector<u32> possibleDirections;
        std::vector<s32> possibleSpeeds;

        // if direction is wildcard, add all possible directions
        if (mWinds[index].mDirection == RPGolDefine::WILDCARD_DIR)
        {
            for (u32 i = 0; i < RPGolDefine::MAX_WIND_DIR; i++)
            {
                possibleDirections.push_back(i);
            }
        }
        else
        {
            possibleDirections.push_back(mWinds[index].mDirection);
        }

        // if speed is wildcard, add all possible speeds
        if (mWinds[index].mSpeed == RPGolDefine::WILDCARD_SPD)
        {
            for (s32 i = 0; i < RPGolDefine::MAX_WIND_SPD; i++)
            {
                possibleSpeeds.push_back(i);
            }
        }
        else
        {
            possibleSpeeds.push_back(mWinds[index].mSpeed);
        }

        // recursively call for each combination of direction and speed
        for (const auto &direction : possibleDirections)
        {
            for (const auto &speed : possibleSpeeds)
            {
                unsigned int combined = (speed & 0xF) << 3 | (direction & 0x7);
                unsigned int newHash = (hash << 7) | combined;
                generateHashes(index + 1, newHash);
            }
        }
    };

    generateHashes(0, 0);
    return hashes;
}

/// <summary>
/// Score a wind set against a target, with the score representing how close
/// it is to its target set. Used to rank seeds in order of how close they are
/// to what wind the user wants.
/// </summary>
/// <param name="target">Target wind set to score against</param>
/// <returns></returns>
Score_t RPGolWindSet::scoreAgainst(const RPGolWindSet &target) const
{
    Score_t myScore = 0;
    for (u32 i = 0; i < RPGolDefine::HOLE_SIZE; i++)
    {
        // Wildcard counts as them matching
        if (mWinds[i].mDirection == RPGolDefine::WILDCARD_DIR)
        {
            myScore += scoreBase;
        }
        // Base score - difference in wind direction
        else
        {
            myScore += scoreBase - std::abs((int)mWinds[i].mDirection - (int)target.mWinds[i].mDirection);
        }
        // The speed score is incremented by 1 and then halved
        // to have about the same priority in the total score as the direction would.
        // (Same wildcard rules apply)
        if (mWinds[i].mSpeed == RPGolDefine::WILDCARD_SPD)
        {
            myScore += scoreBase;
        }
        else
        {
            myScore += scoreBase - (std::abs((int)mWinds[i].mSpeed - (int)target.mWinds[i].mSpeed) + 1) / 2;
        }
    }

    return myScore;
}

/// <summary>
/// Returns data in wind set in string format
/// Custom delimiter support for things like CSV data analysis
/// </summary>
/// <returns>Wind set string</returns>
void RPGolWindSet::toString(char *out, const char *setStartDelim, const char *setEndDelim,
                            const char *termStartDelim, const char *termEndDelim, bool bCloseEndDelim) const
{
    // String buffer
    char buf[1024] = {0};

    // // Insert set start delimiter
    // std::strcat(buf, setStartDelim);

    // Convert each speed + direction to string (keep wildcard if that was part of the input)
    for (u32 i = 0; i < 3; i++)
    {
        // // Term start delim
        // std::strcat(buf, termStartDelim);

        // Wind speed/direction
        char windbuf[128];
        std::snprintf(windbuf, sizeof(windbuf), "%d%s", mWinds[i + 6].mSpeed, RPGolDefine::DirToString(mWinds[i + 6].mDirection));
        std::strcat(buf, windbuf);

        // Term end delim (except last item)
        if (i < 3 - 1 || bCloseEndDelim)
            std::strcat(buf, termEndDelim);
    }

    // // Insert set end delimiter
    // std::strcat(buf, setEndDelim);

    // Copy out full string
    std::strcpy(out, buf);
}