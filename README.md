# MRSample-Unreal

The **MR** sample is a simple Unreal project that demonstrates how to use the Spatial Anchor and the Room Calibration YVR Unreal SDK. You can use this project as a starting point for your own VR applications.

## Getting Started
Connect the YVR device and install the packaged apk, and click to run.

## Mode of operation

**Yvr Button (L) Thumbstick X** -> Player View rotation

**Yvr Button (R) Thumbstick Y** -> Player teleport position, release hand teleport

Yvr controller  is close to the small box/gun on the desktop, **Yvr Button (L/R) Hand Trigger** -> Hold to grab, release to release

For guns, the **Yvr Button (L/R) Index Trigger** -> fires the bullet

The Space Mesh Detector is created when the run starts and destroyed when the **Yvr Button (R) B** is pressed

## Menu

The menu is on the left controller, you can turn it on and off through the menu Button,  align the Settings button you want to click through the right controller,  and press the **Yvr Button (R) Index Trigger** to apply this setting, or move the cursor through the **Yvr Button (L) Thumbstick Y** and apply this setting using the **Yvr Button (L) Index Trigger**:

* Save Auchor: Save anchors to the device's local disk
* Erase Auchor: Clear all anchors in the app
* Show Auchor :  load local device anchors and Generate Spatial Anchor Actor from load result
* Start Capture: evoke the space setup tool for space calibration
* Load Scene:  load local device anchors and and Generate Spatial Anchor Actor, create 2d and 3d actors and modify the scale to create the scene
* Remove Scene Display: Loop through all 2d and 3d actors and destroy it
* Enable Marker: When the equipment is aligned with the marking points, it can display the matching grids.
* Disable Marker: Return to the default scene and clear the grid.
* Restart: Resets the level
* Real life: Quit the game

## Development Environment
| **Name** | **Version** |
| ----  |  ----      |
| Unreal Editor | 5.4.4 |
| YVR Unreal SDK | 3.0.0 |

## Licenses
This project is released under the [MIT License](https://github.com/YVRDeveloper/MRSample-Unreal/blob/main/LICENSE)