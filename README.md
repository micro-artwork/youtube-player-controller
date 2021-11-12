# YouTube Player Controller
This controller is made with Microchip PIC32MX(PIC32MX230F256B) device.

<img src="https://opensourcehardware.io/images/projects/media-controller/IMG_1465.jpg" width="500"/>
<img src="https://opensourcehardware.io/images/projects/media-controller/IMG_1464.jpg" width="500"/>

More details (Korean):
https://opensourcehardware.io/posts/projects/media-controller/2021-11-08-media-controller-dev-01/
https://opensourcehardware.io/posts/projects/media-controller/2021-11-12-media-controller-dev-02/

## Build Environment
* MPLAB X IDE v5.50
* XC32 Compiler v3.01
* MPLAB Harmony 3

## Features
### System Media Control
Control system media such as play/pause, volume increase/decrease, mute and etc.

### YouTube Player Control on Chrome Browser
Control YouTube play/pause, volume, track moving, player size and etc.

Need to specific extension: https://github.com/micro-artwork/youtube-player-controller-chrome-extension

### Key Definition

|Mode/State|Input|Action|
|-|-|-|
|Normal(LED OFF)|Encoder CW|System volume up|
||Encoder CCW|Volume down|
||Encoder Press|Mute|
||Button 1|Move prev track|
||Button 2|Play/pause|
||Button 3|Move next track|
|YouTube (LED ON)|Encoder CW|YouTube volume up|
||Encoder CCW|Volume down|
||Encoder Press|Mute|
||Button 1|Move prev track, if on playlist/prev chapter, if has chpaters|
||Button 2|Play/pause|
||Button 3|Move next track, if on playlist/next recommend video, if single/next chapter, if has chapters|
|YouTube with FN|Fn + Encoder CW|Forward 5 seconds|
||Fn + Encoder CCW|Backward 5 seconds|
||Fn + Encoder Press|Mute (same)|
||Fn + Button 1|Toggle mini player|
||Fn + Button 2|Toggle theater mode|
||Fn + Button 3|Toggle full screen|
|Regardless of Status|Button 4|Function|
||Side Button|Mode Change|


## Demo
Tested on Chrome Broswer and Windows 10

### Normal Mode
![demo](https://opensourcehardware.io/images/projects/media-controller/demo_01.gif)

### YouTube Mode
![demo](https://opensourcehardware.io/images/projects/media-controller/demo_02.gif)

### Full Video

https://youtu.be/gxZb6ow81-E

https://youtu.be/KZX7UcYx0UY



