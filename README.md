# Goldfish

A REST API for predicting wind in golf for games in the Wii Sports series.

## Demo

https://goldfish-demo.vercel.app

https://github.com/purpuropterus/goldfish-demo

## Precompute files

### Download

#### Wii Sports 1.0

`og_wind_precompute_1.0.bin` (34 KB): https://app.drime.cloud/drive/s/4HqfLSxc5F2WCQM8sBFP8KdjReG9EI

-   Due to a bug in the `RPUtlRandom::getF32()` function in 1.0, there are much less effective RNG seeds than there should be for the purposes of generating wind sets. It was fixed in 1.1.

#### Wii Sports 1.1/1.2

`og_wind_precompute.bin` (8.2 GB): https://app.drime.cloud/drive/s/Ymin69vHRnQxMpyt9fHOgCRTYradFX

#### Wii Sports Resort

`wsr_wind_precompute.bin` (7.4 GB): https://app.drime.cloud/drive/s/gq1Bt63vWOcanP8HmGPjHSYcYF7VTr

### Generation

The code for generating the precompute files are also available in this repository under [`server/src/core/generators`](server/src/core/generators).

```sh
cd server/src/core
cmake -S . -B build
cmake --build build
./build/generator
```

## Running

### Development

```sh
cd server
export OG_WIND_PRECOMPUTE_PATH_1_0=/path/to/og_wind_precompute_1.0.bin
export OG_WIND_PRECOMPUTE_PATH_1_1=/path/to/og_wind_precompute.bin
export WSR_WIND_PRECOMPUTE_PATH=/path/to/wsr_wind_precompute.bin
cargo run
```

### Production

```sh
sudo docker build -t goldfish .
sudo docker run \
    -p [PORT]:3000 \
    -v /path/to/og_wind_precompute_1.0.bin:/og_wind_precompute_1.0.bin \
    -v /path/to/og_wind_precompute.bin:/og_wind_precompute_1.1.bin \
    -v /path/to/wsr_wind_precompute.bin:/wsr_wind_precompute.bin \
    goldfish
```

## Structure of the project

-   [server](server/): Rust code that implements the REST API using [axum](https://github.com/tokio-rs/axum). Interfaces with the `core` module using [cxx](https://github.com/dtolnay/cxx).
-   [server/src/core](server/src/core/): C++ code that implements the core logic.
    -   [/generators](server/src/core/generators/): Generating precompute files
    -   [/finders](server/src/core/finders/): Finding RNG seeds that match a partially known wind. Partially based on [kiwi515/GolfAtHome](https://github.com/kiwi515/GolfAtHome).

## Special thanks

-   [kiwi515](https://github.com/kiwi515/)
    -   for [GolfAtHome](https://github.com/kiwi515/GolfAtHome)
    -   for an initial attempt at a web-based wind bruteforcer that I took some ideas from
