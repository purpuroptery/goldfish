#include "RPUtlRandom.h"
#include "lib/rvl/OSTime.h"

#include <iostream>

/// <summary>
/// Initialize seed using OSCalendarTime object
/// </summary>
/// <param name="ctime">OSCalendarTime reference</param>
void RPUtlRandom::initialize(const OSCalendarTime &ctime)
{
    GetInstance()->mSeed = (ctime.min << 26 | ctime.sec << 20 | ctime.msec << 10 | ctime.usec);
}

/// <summary>
/// Initialize seed using 32-bit seed
/// </summary>
/// <param name="seed">Seed value</param>
void RPUtlRandom::initialize(u32 seed)
{
    GetInstance()->mSeed = seed;
}

/// <summary>
/// Get RNG seed
/// </summary>
/// <returns>32-bit seed</returns>
u32 RPUtlRandom::getSeed()
{
    return GetInstance()->mSeed;
}

/// <summary>
/// Advance seed a specified number of steps
/// </summary>
/// <param name="n">Number of steps to advance</param>
void RPUtlRandom::advance(u32 n)
{
    for (u32 i = 0; i < n; i++)
        calc();
}

void RPUtlRandom::unadvance(u32 n)
{
    RPUtlRandom *rng = GetInstance();

    for (u32 i = 0; i < n; ++i)
    {
        rng->mSeed = (rng->mSeed - 1) * SEED_STEP_INV;
    }
}

/// <summary>
/// Get random u32 value
/// </summary>
/// <returns>Random u32</returns>
u32 RPUtlRandom::getU32()
{
    return calc();
}

/// <summary>
/// Get random float value
/// </summary>
/// <returns>Random f32</returns>

f32 RPUtlRandom::getF32(bool ver_1_0)
{
    u32 seed = getU32();

    // std::cout << ver_1_0 << std::endl;

    // in 1.0, the bottom 16 bits of the seed are used
    // in 1.1/1.2, the top 16 bits are used
    if (!ver_1_0)
    {
        seed >>= 16;
    }

    // Limited to u16 bounds
    const u16 rnd = static_cast<u16>(0xFFFF & seed);
    // Convert to float
    const f32 rnd_f = static_cast<f32>(rnd);
    // Convert to percentage
    return rnd_f / static_cast<f32>(0xFFFF + 1);
}

/// <summary>
/// Advance seed one step forward, and return its value
/// </summary>
/// <returns>Seed after stepping forward one iteration</returns>
u32 RPUtlRandom::calc()
{
    RPUtlRandom *rng = GetInstance();

    // Linear congruential generator implementation
    // Old versions of glibc also use 69069 as the multiplier
    // u64 used to replicate overflow
    u64 seed = rng->mSeed * SEED_STEP + 1;
    return rng->mSeed = (u32)seed;
}