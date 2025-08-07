# Compiling S60Maps

This guide will describe you how to compile S60Maps project from sources on your computer. This may be usefull, for example, for translators to have a preview of their translations.

In this tutorial I will use Windows 7 and SDK S60 v3.1 (Symbian 9.2). I recommend to use this version of SDK, but you may install another one which you can found [here](https://www.mediafire.com/folder/v862wwqds229r/SDKs).

Also works with Windows XP. Not sure about modern Windows versions. Anyway you can use virtual machine (like VirtualBox) if you use another OS (e.g., Linux).

# Installation


## Java Runtime Environment

If you already have Java Runtime Environment (JRE) installed you may skip this step. For download follow [this link](https://www.java.com/en/download/windows_offline.jsp).


## S60 SDK

[Download S60 v3.1 SDK](http://www.mediafire.com/file/9uc7fjb2ynmxlud/s60v3.1_SDK.zip/file) and unpack zip to any folder. Run `setup.exe`.

First Perl 5.6.1 setup will be run. Leave options as is.

![][1]

After that installation of SDK will be started. Upon completion you will be asked about install compiling toolchine. Press `Yes`.

![][2]


## Carbide C++ IDE

Download and install [Carbide.C++ v3.2 IDE](https://mega.nz/#!Kt1wkArK!Aylrqj6x7LOw5DxdfJGbEeRK114698t-FuTV3MVzxSA) ([v2.7](http://www.mediafire.com/file/6z54qrceef73x9s/Carbide_cpp_v2_7_en.exe/file) also should works).

After the install is complete start Carbide C++ IDE. At the first run you will be asked about workspace directory in which projects will be stored. You may leave it as default.

![][3]

Also restart of IDE may be needed.

![][4]


## Git (optional)
It's recommended, but not necessary to have [Git](https://git-scm.com/downloads) version control system installed.


# Build project in Carbide C++ IDE

## Import project to Carbide C++ IDE

- **If you use Git** go to workspace directory `C:\Symbian\Carbide\workspace` and clone repository with `--recurse-submodules` option to load all submodules too:
  ```
  git clone --recurse-submodules https://github.com/artem78/s60-maps.git
  ```
  Swith to main development branch called `develop`:
  ```
  git checkout develop
  ```
  
- **If you are not use Git** you may manualy download complete sources of last stable version [from release page](https://github.com/artem78/s60-maps/releases/latest). In `Asserts` section find zip file named `s60-maps_sources_vX.X.X.zip`. Unpack its content to `C:\Symbian\Carbide\workspace\s60-maps\` directory.

  ![][5]

Go to Carbide IDE. Open `File > Import`. Select `General > Existing Projects into Workspace` and press `Next`.

![][6]

Click `Browse...` button and choose S60Maps project directory.

![][7]

Press `Finish`.

## Build and run

There are two build modes:
1. For emulator (WINSCW platform)
2. For real phone (GCCE platform)

You can choose between them in `Project > Build Configurations > Set Active`.

![][8]


## Run in emulator

Set `Emulator Debug (WINSCW) [S60_3rd_FP1]` build mode and press `Run` button:

![][9]

If compilation finished without errors emulator will be started with running S60Maps application:

![][10]


## Make SIS(X) installation package for phone

Read [this note](#do-not-forget-to-set-api-keys-for-release-build-error) first.

Set `Phone Release (GCCE) [S60_3rd_FP1]` build mode. Select `Project > Build Project` or `Project > Clean...` for force update all resources.

After successful compilation you will find `S60Maps.sis` (unsigned) and `S60Maps.sisx` (self-signed) files in `sis` folder.


# How to?

## Emulator trial reset

Emulator have 14 days trial period, but you can reset it unlimited number of times. Use [this BAT-script](https://gist.github.com/artem78/53c6529794c1c98fb640d1c7560fbaa5#file-nokia_registration_reset-bat) or directly remove `HKCU\Software\Nokia\Product Registration` subtree in `regedit`.

## "Do not forget to set API keys for release build!" error

When you try to build `Release` build typically you should recieved this error:

> #error Todo: Do not forget to set API keys for release build!

OpenCycleMap and Transport map layers requires API key. My own key do not publicly stored in git for security reason, but you can get you own for free [from Thunderforest](https://manage.thunderforest.com/dashboard).

Another way - you may leave this key empty, but in this case you may see "api key required" message on OpenCycleMap and Transport Map tiles.

Key defined in `inc/ApiKey.h`

```
#ifndef APIKEYS_H_
#define APIKEYS_H_

_LIT8(KThunderForestApiKey, ""); // leave empty string or put your own key

#endif /* APIKEYS_H_ */
```

Don't forget to delete `#error ...` statement for success build.

## Fix other problems

https://gist.github.com/artem78/cb2b9650af186844f7b5654964676284#howto

[1]: images/compiling/20211201_172149.png
[2]: images/compiling/20211201_172734_2.png
[3]: images/compiling/20211201_173030_2.png
[4]: images/compiling/20211201_173219_2.png
[5]: images/compiling/release_2.png
[6]: images/compiling/20211201_173339.png
[7]: images/compiling/20211201_173422_2.png
[8]: images/compiling/20211201_173521_2.png
[9]: images/compiling/20211210_163048_2.png
[10]: images/compiling/20211201_173754.png

