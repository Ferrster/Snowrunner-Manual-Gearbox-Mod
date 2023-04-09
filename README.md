# SnowRunner Manual Gearbox Mod

Disables 'auto' gear shifting in favor of manual shifting through custom keybindings for keyboard and gamepads.

**Currently Logitech Driving Force Shifter and other joystick-type devices are not supported, but I'm working on it.**

## Installation

1. Get the latest release [here](https://github.com/Ferrster/Snowrunner-Manual-Gearbox-Mod/releases)
1. Extract _.dll_ and _.exe_ files from archive into the game folder, where _SnowRunner.exe_ is located
1. Start SnowRunner
1. Run _smgm-loader.exe_ to apply the mod

## Usage

When you use the mod for the first time, it'll create a config file _(smgm.ini)_ near game's main _.exe_ file.
There you can change keybinding for keyboard and for gamepad separately. By default, cycling through gears is done by **LCtrl** and **LAlt** on keyboards, and **DPad Left & Right** on gamepads. Keys are represented by hex values.

You can find all values for keyboard keys in [Microsoft docs](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).

Available gamepad keys:
```c++
#define VK_PAD_A                        0x5800
#define VK_PAD_B                        0x5801
#define VK_PAD_X                        0x5802
#define VK_PAD_Y                        0x5803
#define VK_PAD_RSHOULDER                0x5804
#define VK_PAD_LSHOULDER                0x5805
#define VK_PAD_LTRIGGER                 0x5806
#define VK_PAD_RTRIGGER                 0x5807

#define VK_PAD_DPAD_UP                  0x5810
#define VK_PAD_DPAD_DOWN                0x5811
#define VK_PAD_DPAD_LEFT                0x5812
#define VK_PAD_DPAD_RIGHT               0x5813
#define VK_PAD_START                    0x5814
#define VK_PAD_BACK                     0x5815
#define VK_PAD_LTHUMB_PRESS             0x5816
#define VK_PAD_RTHUMB_PRESS             0x5817

#define VK_PAD_LTHUMB_UP                0x5820
#define VK_PAD_LTHUMB_DOWN              0x5821
#define VK_PAD_LTHUMB_RIGHT             0x5822
#define VK_PAD_LTHUMB_LEFT              0x5823
#define VK_PAD_LTHUMB_UPLEFT            0x5824
#define VK_PAD_LTHUMB_UPRIGHT           0x5825
#define VK_PAD_LTHUMB_DOWNRIGHT         0x5826
#define VK_PAD_LTHUMB_DOWNLEFT          0x5827

#define VK_PAD_RTHUMB_UP                0x5830
#define VK_PAD_RTHUMB_DOWN              0x5831
#define VK_PAD_RTHUMB_RIGHT             0x5832
#define VK_PAD_RTHUMB_LEFT              0x5833
#define VK_PAD_RTHUMB_UPLEFT            0x5834
#define VK_PAD_RTHUMB_UPRIGHT           0x5835
#define VK_PAD_RTHUMB_DOWNRIGHT         0x5836
#define VK_PAD_RTHUMB_DOWNLEFT          0x5837
```

To apply new changes you must restart the mod.

* Press **F1** to detach the mod from the game (you will see `DLL Detach` message in the console)
* Close console window
* Run _smgm-loader.exe_ again



## Building

Todo...
