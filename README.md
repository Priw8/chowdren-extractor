# Chowdren extractor

This is a tool to extract data from the `Assets.dat` file of games that use [Chowdren](https://mp2.dk/chowdren/). It was originally written just with Cyber Shadow in mind, but it has since evolved far beyond the original scope.

## TODO

- Allow extracting files from the `font`, `files` and `platform` sections (what does `platform` even mean?).
- Check if platform-specifics format exist and make them compatible if they do
- An option to repack the data, somehow

## What you need to build

- The Boost library
- The zlib library
- [meson](https://mesonbuild.com/) and [ninja](https://github.com/ninja-build/ninja/)*

\* ninja is only needed if you want to use it to build. You can apparently also generate vs and xcode projects, but I never used that. Instructions below assume ninja.

## How to build

1. Clone the repo
2. cd into the repo and run `git submodule init && git submodule update` 
3. run `cd src`
4. run `meson setup builddir`
5. `cd builddir` and then run `ninja` and it should build, hopefully. If not, you're probably on your own

## How to use

Run `./chowdren-extractor --help` for info. 
```
Usage: chowdren-extractor [options] input.dat output-dir
Named options:
  --format arg          see versions.md for details
  --probe-offsets       only find offsets and exit
  --no-images           skip extracting images
  --no-audio            skip extracting audio
  --no-shaders          skip extracting shaders
  --help                print help message
```

Note that there are no filenames included in the Assets file, so files are just extracted as `image1.png`, `audio1.ogg` etc.

## Archive versions

See [versions.md](versions.md) for more details.
