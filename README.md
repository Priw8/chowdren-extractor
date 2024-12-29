# Chowdren extractor

[![Build status](https://ci.appveyor.com/api/projects/status/em960g3q0960ai0b?svg=true)](https://ci.appveyor.com/project/Priw8/chowdren-extractor)

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

### Repo setup

1. Clone the repo
2. cd into the repo and run `git submodule init && git submodule update`

### If building under Linux (or probably any Unix-like)

1. Run `cd src`
2. Run `meson setup builddir`
3. `cd builddir` and then run `ninja` and it should build, hopefully. If not, you're probably on your own

### If building under Windows (MSVC)

1. Ensure that you have the Visual Studio toolchain installed
2. Open `Developer Command Prompt for VS 20XX` (whichever version you have)
3. In the developer command prompt navigate to the repo and enter the `src` folder
4. Run `meson setup builddir --vsenv`
5. Run `meson compile -C builddir` and pray (important)

#### Windows troubleshooting
- Meson can't find the Boost library: set the `BOOST_ROOT` environment variable to wherever you have it installed 
(for example: `set BOOST_ROOT=C:\Libraries\boost_1_83_0`)
- Meson can't find the zlib library: try installing it with `meson wrap`. To do that:
  - Create a directory called `subprojects` inside of `src`
  - While in `src`, run `meson wrap install zlib`
- The binary got built but running it gives missing DLL errors when running:
  - `z-1.dll` - copy it from `src/builddir/subprojects/zlib-x.x.x/z-1.dll`
  - `boost_whatever.dll` - it should be prebuilt somewhere in your Boost installation (check `lib64-msvc-x` folders for 64bit builds and `lib32-msvc-x` folders for 32bit builds)
  - Debug DLLs such as `MSVCP140D.dll` (notice the `D` at the end) - if you don't have them someone else probably built the program and sent it to you, they should build it in release mode instead (append `--buildtype release` flag when running `meson setup`).
If you built the program yourself and they are missing, I don't know how that's possible but something is very wrong about the Visual Studio installation.
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
