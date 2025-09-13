#pragma once

#include "ExactMatchFinder.h"

#include "lib/RP/RPGolWindSet.h"
#include "lib/RP/RPGolDefine.h"
#include "lib/RP/RPGolConfig.h"
#include "lib/RP/RPUtlRandom.h"
#include "lib/RP/RPGolDifficulty.h"

struct OgWindFinderOutput
{
    u32 seed;
    RPGolWindSet windSet;
};

class OgWindFinder : public ExactMatchFinder<RPGolWindSet, OgWindFinderOutput>
{
public:
    OgWindFinder(const std::string &filePath, bool ver_1_0)
        : ExactMatchFinder<RPGolWindSet, OgWindFinderOutput>(ver_1_0 ? 1 << 7 : 1 << 21, filePath) {}

    virtual ~OgWindFinder() = default;

    virtual std::vector<u32> inputToHashes(const RPGolWindSet &input) override
    {
        return input.hashesWithDepth(ver_1_0 ? 1 : 3);
    }

    virtual OgWindFinderOutput generatePotentialOutputFromSeed(u32 seed, const RPGolWindSet &input) override
    {
        RPUtlRandom::initialize(seed);
        auto windSet = RPGolWindSet();
        RPGolConfig::getInstance()->MakeWindSet(diff_Expert, windSet, ver_1_0);
        return {seed, windSet};
    }

    virtual bool doesPotentialOutputMatchInput(const OgWindFinderOutput &output, const RPGolWindSet &input) override
    {
        return true;
    }

    virtual u32 nextSeed(u32 currentSeed) override
    {
        RPUtlRandom::initialize(currentSeed);
        RPUtlRandom::advance(1);
        return RPUtlRandom::getSeed();
    }

private:
    bool ver_1_0 = true;
};