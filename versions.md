# Assets.dat format versions

Various games use slightly different internal formats for entries in `Assets.dat`. The table below attempts to document them.

Listed below is every game mentioned on the [chowdren website](https://mp2.dk/chowdren/), sorted by the release date I could find. Not all of them use the archive format.

Platform names:
- Win: Windows
- Lnx: Linux
- CTR: Nintendo 3DS

| Release date | Game                         | Has Assets.dat | Format      | Image format | Sound format | Shader format | Entry type count |
|--------------|------------------------------|----------------|-------------|--------------|--------------|---------------|------------------|
| 2013-01-28   | The Great Work (Win)         | No             | N/A         | N/A          | N/A          | N/A           | N/A              |
| 2013-10-25   | Knytt Underground (Win)      | No             | N/A         | N/A          | N/A          | N/A           | N/A              |
| 2015-04-02   | AVGN Adventures (CTR)        | Yes            | v?          | ?/PNG        | ?            | ?             | ?                |
| 2015-02-13   | The Escapists (Lnx)          | Yes            | v0          | v0/zlib      | v1           | pair          | 5                |
| 2015         | Freedom Planet (Lnx 2015ver) | Yes            | v0          | v0/zlib      | v1           | pair          | 5                |
| 2015         | NOT A HERO (Lnx)             | Yes            | v0          | v0/zlib      | no sounds    | pair          | 5                |
| 2016         | Noitu Love: Devolution (CTR) | Yes            | v0-ctr-zlib | v1/zlib      | v0           | blob          | 5                |
| 2018         | Dodge Club Pocket (CTR)      | Yes            | v0-ctr-chow | v1/chowimg   | v0           | blob          | 5                |
| 2018         | Iconoclasts (Win)            | Yes            | v1          | v2/zlib      | v1           | pair          | 6                |
| 2019         | Baba Is You (Lnx)            | Yes            | v2          | v1/chowimg   | v1           | pair          | 6                |
| 2020         | Petal Crash (Lnx)            | Yes            | v3          | v3/zlib      | v2           | pair          | 6                |
| 2021         | Cyber Shadow (Lnx)           | Yes            | v3          | v3/zlib      | v2           | pair          | 6                |
| 2021         | Mighty Goose (Win)           | Yes            | v3          | v3/zlib      | v2           | pair          | 6                |
| 2022         | OMORI (Win)                  | No..?          |             | ?            | ?            | ?             | ?                |

See below for game-specific details.

## The Great Work 

This is a [free](https://nifflas.ni2.se/?page=The+Great+Work) game that does not use `Assets.dat` (or any other archive format). I was able to locate only the Windows version, but I didn't really search much. I also think it's unlikely that the Linux/OSX version differs in terms of data storage.

## Knytt Underground

Available on Windows, Linux, OSX, PS3, Vita and Wii U. Curiously, the Chowdren website does not mention PS3 or Vita.

| Platform | Checked | Notes                                                                              |
|----------|---------|------------------------------------------------------------------------------------|
| Windows  | no      |                                                                                    |
| Linux    | yes     | No `Assets.dat` present; file structure looks similar to that of *The Great Work*. |
| OSX      | no      |                                                                                    |
| PS3      | no      |                                                                                    |
| Vita     | no      |                                                                                    |
| Wii U    | no      |                                                                                    |

## Angry Video Game Nerd Adventures

Originally released in 2013 on Steam, but the Chowdren website has a 2015 date and lists 3DS and Wii U; I think it's fair to assume that Chowdren was only used for the console ports. 

| Platform | Checked | Notes                                                       |
|----------|---------|-------------------------------------------------------------|
| Windows  | Yes     | No `Assets.dat` present                                     |
| 3DS      | Yes     | Extracted RomFS contains an unknown version of `Assets.dat` |
| Wii U    | No      |                                                             |

It appears that the `Assets.dat` here has quite large differences compared to the later ones; It's unsupported by the program at the time being. It might require reverse engineering the binary to understand. It's also possible that it's merely missing the `type_sizes` struct, which means that offsets cannot be figured out by the probing mechanism.

## The Escapists

Chowdren website lists only Linux and MacOS versions, which is interesting. Was it only used to port the game from Windows?

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | No      |
| Linux    | Yes     | `v0`    |
| OSX      | No      |

## Freedom Planet

This game annoyingly uses different formats depending on the version.

| Platform | Checked | Version                                       |
|----------|---------|-----------------------------------------------|
| Windows  | Partial | `v0` for original version; later releases TBD |
| Linux    | No      |                                               |
| OSX      | No      |                                               |
| PS4      | No      |                                               |
| Wii U    | No      |                                               |
| Switch   | No      |                                               |

## Not a Hero

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | No      |         |
| Linux    | Yes     | `v0`    |
| OSX      | No      |         |
| PS4      | No      |         |
| Xbox One | No      |         |
| Switch   | No      |         |
| Android  | No      |         |

## Dodge Club Pocket

Requires extracting the RomFS to see the files it has.

| Platform | Checked | Version       |
|----------|---------|---------------|
| 3DS      | Yes     | `v0-ctr-chow` |

## Noitu Love: Devolution
Requires extracting the RomFS to see the files it has.

| Platform | Checked | Version       |
|----------|---------|---------------|
| 3DS      | Yes     | `v0-ctr-zlib` |
| Wii U    | No      |               |

## Iconoclasts

Chowdren website does not list the Xbox One version.

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | Yes     | `v1`    |
| Linux    | No      |         |
| OSX      | No      |         |
| PS4      | No      |         |
| Vita     | No      |         |
| Switch   | No      |         |
| Xbox One | No      |         |

## Baba is You

Chowdren website does not list the mobile versions.

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | No      |         |
| Linux    | Yes     | `v2`    |
| OSX      | No      |         |
| Switch   | No      |         |
| IOS      | No      |         |
| Android  | No      |         |

## Petal Crash

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | No      |         |
| Linux    | Yes     | `v3`    |
| OSX      | No      |         |
| PS4      | No      |         |
| Switch   | No      |         |

## Cyber Shadow

| Platform | Checked | Version |
|----------|---------|---------|
| Windows  | No      |         |
| Linux    | Yes     | `v3`    |
| OSX      | No      |         |
| PS4      | No      |         |
| PS5      | No      |         |
| Xbox One | No      |         |
| Switch   | No      |         |

## Mighty Goose

| Platform  | Checked | Version |
|-----------|---------|---------|
| Windows   | Yes     | `v3`    |
| Linux     | No      |         |
| OSX       | No      |         |
| PS4       | No      |         |
| PS5       | No      |         |
| Xbox One  | No      |         |
| Xbox SX/S | No      |         |
| Switch    | No      |         |

## OMORI

This requires more research.

| Platform | Checked | Notes                     |
|----------|---------|---------------------------|
| Windows  | Yes     | No `Assets.dat` present?? |
| Other    | No      |                           |

# Image formats

Images stored in the archives are known to be compressed in one of 2 ways:
- `zlib`
- `chowimg` (custom compression made specifically for chowdren)

As far as I can tell, there is no clear pattern for when which method is used.

Apart from compression, image entries also differ; so far there are 4 known formats which I called `v0`, `v1`, `v2` and `v3` (didn't have better names).

# Sound formats

There are 3 known containers that entries can use:
- `1` - wav
- `2` - ogg
- `3` - [cwav](https://www.3dbrew.org/wiki/BCWAV) (3DS specific)

Fortunately, entries themselves contain the ID for which container is used. However, said entries come in 3 different flavors: `v0`, `v1` and `v2`. 

It is however possible that `v0` and `v1` are the same, and the extra data that is thought to be a part of `v0` is actually extra metadata that's placed specifically when container type is `3` (cwav). Unfortunately, there is currently no way to tell for sure, as it'd require a file that mixes `cwav` and another container type, and as far as I can tell, there isn't one like that.

# Shader formats

Shaders are stored in a platform-native format. Each entry usually contains a pair of shaders, but it may only contain one if the format is capable of storing both the vertex and fragment shader, in which case it is represented as `blob` in the table.