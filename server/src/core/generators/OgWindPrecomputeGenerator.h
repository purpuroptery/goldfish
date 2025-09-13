#include "AbstractPrecomputeGenerator.h"

#include "lib/RP/RPGolDifficulty.h"
#include "lib/RP/RPGolConfig.h"
#include "lib/RP/RPUtlRandom.h"
#include "lib/RP/RPGolWindSet.h"

class OgWindPrecomputeGenerator : public AbstractPrecomputeGenerator
{
public:
    OgWindPrecomputeGenerator(bool ver_1_0) : AbstractPrecomputeGenerator(ver_1_0 ? 1 << 7 : 1 << 21) {}

    virtual ~OgWindPrecomputeGenerator() = default;

    virtual u32 seedToHash(u32 seed) override
    {
        auto windSet = RPGolWindSet();
        RPUtlRandom::initialize(seed);
        RPGolConfig::getInstance()->MakeWindSet(diff_Ninehole, windSet, false);
        return windSet.hashesWithDepth(ver_1_0 ? 1 : 3)[0];
    }

    virtual u32 nextSeed(u32 currentSeed) override
    {
        return currentSeed + 1;
    }

private:
    bool ver_1_0;
};