# event-player

Record your touch operation and play it again on your Android device.

## Requirement
- Android NDK tool set (https://developer.android.com/ndk/)

## Getting Started

1. Start event-recorder  (`./event-recorder`)
2. Operate your device
3. Stop event-recorder w/ Ctrl-C, then event-player-xxxxx.sh is created
   And follow the instruction displayed like below

```
    adb push $COREPLAYER_NAME /data/local/tmp;
    adb shell chmod 777 /data/local/tmp/$COREPLAYER_NAME
    adb push $record_name /data/local/tmp/
```

## Build event-player core

```
    $ cd player/
    $ ndk-build
```

## Restrictions
- Only support following touchpad device names; 
  - clearpad
  - max1187x_touchscreen_0

  However you can add a touchpad of your device by editing @TOUCHPAD_DEV_NAME
  in event-recorder
