#include "OgWindFinder.h"
#include "WsrWindFinder.h"
#include "OgBlinkFinder.h"
#include "BlinkGroup.h"

#include "WindArgParser.h"

#include <iostream>
#include <sstream>
#include <iomanip>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <mode> <precompute_file> <wind_string>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string filePath = argv[2];
    std::string windStr = argv[3];

    switch (mode[0])
    {
    case 'o':
    {
        std::cout << "Using OgWindFinder mode" << std::endl;

        RPGolWindSet windSet(9);
        if (!WindArgParser::parseTargetWindSet(windStr, windSet))
        {
            std::cerr << "Error: Invalid wind argument" << std::endl;
            return 1;
        }

        OgWindFinder finder(filePath, true);
        auto results = finder.find(windSet);

        char buffer[1024] = {0};

        for (const auto &result : results)
        {
            std::cout << std::hex << std::setw(4) << std::setfill('0') << result.seed;
            result.windSet.toString(buffer);
            std::cout << "," << buffer << "\n";
        }

        break;
    }
    case 'w':
    {
        std::cout << "Using WsrWindFinder mode" << std::endl;
        RPGolWindSet windSet(21);
        if (!WindArgParser::parseTargetWindSet(windStr, windSet))
        {
            std::cerr << "Error: Invalid wind argument" << std::endl;
            return 1;
        }

        WsrWindFinder finder(filePath);
        auto results = finder.find(windSet);

        char buffer[1024] = {0};

        for (const auto &result : results)
        {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << result.seed << std::endl;
            result.windSet.toString(buffer);
            std::cout << "," << buffer << "\n";
        }
        break;
    }
    case 'b':
    {
        std::cout << "Using OgBlinkFinder mode" << std::endl;

        // get comma separated numbers from windStr
        std::vector<u32> blinkGroupNumbers;
        std::stringstream ss(windStr);
        std::string number;
        while (std::getline(ss, number, ','))
        {
            blinkGroupNumbers.push_back(std::stoul(number));
        }

        BlinkGroup blinkGroup(blinkGroupNumbers);

        OgBlinkFinder finder(filePath);
        auto results = finder.find(blinkGroup);

        for (const auto &result : results)
        {
            std::cout << "Seed: 0x" << std::hex << result.output.seed << " : " << result.score << std::endl;
        }

        break;
    }
    default:
        std::cerr << "Invalid mode. Modes: o,w,b" << std::endl;
        return 1;
    }

    return 0;
}