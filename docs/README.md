# S60Maps

Simple map for Symbian phones. Supports s60v3 and s60v5.

![About dialog](images/about.png)

## Features
* Show map from default [OpenStreetMap](https://www.openstreetmap.org/) layer
* Retrieve phone location using internal GPS
* **Offline mode** - all downloaded tiles save in cache on disk and you can view them later without network connection needed

## Controls

### Keys
* Move: `arrows` or `2`, `4`, `6`, `8`
* Zoom: `1` and `3` or volume keys

### Touchscreen
* Touch top/bottom/left/right side of the screen
to move up/down/left/right
* Touch and hold to move continuously
* Swipe left/right to zoom out/in
* Swipe up/down to show/hide softkeys (Options | Exit)

## Screenshots

![Menu](images/menu.png) ![Application launch icon](images/launch_icon.png) ![Demo](images/demo_video.gif)

## Download
Download and install `sis` or `sisx` package from [release page](../../../releases/latest/).

Must be signed or installed on unlocked phone.

## Task list
- [ ] Add support for other map layers/services (like OSM bicycle, OSM humanitarian, OpenTopoMap, etc...) and add ability to define custom providing tile\`s URLs
- [ ] Update old cached tiles
- [ ] Display landmarks
- [ ] Search (using [Nominatim](https://nominatim.openstreetmap.org/))
- [ ] Show satellites info (amount, signal strength, etc...)

## Testing
Tested on:
* Nokia N95 8GB
* Nokia 5530 XM (thanks to [baranovskiykonstantin](https://github.com/baranovskiykonstantin))

If you can test it works (or not) on other s60-based phones, let me know.

## License
[GNU GPL v3.0](/LICENSE.txt)
