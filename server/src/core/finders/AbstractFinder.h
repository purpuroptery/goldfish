#pragma once

#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>

#include "types.h"

#include "util_zlib.h"

#include "lib/RP/RPUtlRandom.h"

template <typename TInput, typename TOutput>
class AbstractFinder
{
public:
    AbstractFinder(u32 numHashes, const std::string &filePath)
        : numHashes(numHashes), filePath(filePath) {}

    ~AbstractFinder() {}

    std::vector<u32> getSeedsFromFile(const std::string &filePath, std::vector<u32> hashes)
    {
        std::vector<u32> seeds;

        if (hashes.empty())
        {
            return seeds;
        }

        std::ifstream file(filePath, std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Error: Could not open file " + filePath);
        }

        for (const auto &hash : hashes)
        {

            if (hash >= numHashes)
            {
                throw std::out_of_range("Hash " + std::to_string(hash) + " is out of range for this generator that expects " + std::to_string(numHashes) + " hashes.");
            }

            // seek `hash` number of u64s + 1 u32 for number of offsets from beginning of file
            file.seekg(hash * sizeof(u64) + sizeof(u32), std::ios::beg);

            // read the offset
            u64 offset;
            file.read(reinterpret_cast<char *>(&offset), sizeof(u64));

            // std::cout << offset << std::endl;

            // read the next offset to determine the size of the compressed data
            u64 next_offset;
            file.read(reinterpret_cast<char *>(&next_offset), sizeof(u64));

            // std::cout << next_offset << std::endl;

            u64 difference = next_offset - offset;

            // std::cout << difference << std::endl;

            // if there's no seeds for this hash, continue
            if (difference < sizeof(u64))
            {
                continue;
            }

            // std::cout << hash << std::endl;

            u64 size = difference - sizeof(u64);

            // seek `offset` amount of bytes ahead
            file.seekg(offset - (2 * sizeof(u64)), std::ios::cur);

            // you will now be at the location of the compressed data
            // uncompress the data
            std::vector<u8> compressedData(size);
            file.read(reinterpret_cast<char *>(compressedData.data()), size);

            std::vector<u8> decompressedData;
            decompressData(compressedData, decompressedData);

            // number of seeds is the first 4 bytes of the uncompressed data
            u32 seeds_size = 0;
            for (int i = 0; i < 4; ++i)
            {
                seeds_size |= (decompressedData[i] << (8 * i));
            }

            // construct the seeds array (the data is stored MSB->LSB)
            std::vector<u32> thisHashSeeds(seeds_size);

            int read_pos = 4;

            for (int i = 0; i < 4; i++)
            {
                for (u32 j = 0; j < seeds_size; j++)
                {
                    thisHashSeeds[j] |= (decompressedData[read_pos] << (8 * (3 - i)));
                    read_pos++;
                }
            }

            // un-delta encode the seeds
            for (u32 i = 1; i < thisHashSeeds.size(); i++)
            {
                thisHashSeeds[i] += thisHashSeeds[i - 1];
            }

            // add thisHashSeeds to seeds
            seeds.insert(seeds.end(), thisHashSeeds.begin(), thisHashSeeds.end());
        }
        return seeds;
    }

    std::vector<TOutput> find(const TInput &input, s64 last_known_seed = -1, u32 num_to_check = 0)
    {
        std::vector<u32> seeds;

        // if there's a last known seed, generate the next `num_to_check` seeds
        if (last_known_seed != -1)
        {
            seeds = std::vector<u32>(num_to_check);
            u32 seed = (u32)(last_known_seed);
            for (u32 i = 0; i < num_to_check; ++i)
            {
                seed = this->nextSeed(seed);
                seeds[i] = seed;
            }
        }
        // if there's no last known seed, read the seeds from the precompute file
        else
        {
            // std::vector<u32> hashes = inputToHashes(input);
            // seeds = getSeedsFromFile(filePath, hashes);

            seeds.resize(65536);
            for (u32 i = 0; i < 65536; ++i)
            {
                seeds[i] = i;
            }
        }

        std::vector<TOutput> results = getResults(seeds, input);

        // std::cout << seeds.size() << std::endl;
        // for (u32 seed : seeds)
        //     std::cout << "Seed: 0x" << std::hex << seed << std::endl;

        return results;
    }

    virtual std::vector<u32> inputToHashes(const TInput &input) = 0;
    virtual TOutput generatePotentialOutputFromSeed(u32 seed, const TInput &input) = 0;

    virtual std::vector<TOutput> getResults(const std::vector<u32> &seeds, const TInput &input) = 0;

    virtual u32 nextSeed(u32 currentSeed) = 0;

protected:
    u32 numHashes;
    std::string filePath;
};