# AutomatedChargingOutlet
Firmware for esp32-based smart outlet that snoozes when connected to over bluetooth or wifi, allowing for easy automation of smartphone charge limiting

Currently the outlet turns its relay off for 30 minutes whenever:
 - it has a GAP event of any kind
 - a device connects to it's access point

The intent for this project is for one to set up an automation on one's phone (using tasker for example) to detect when the phone is at 80%, and then connect to this device to turn charging off temporarily.

## Installing

Change the pin assignments as necessary, and then connect to the device and upload using whatever means one would like.

Once this firmware is uploaded, it will throw up an access point, on which it exposes arduino OTA without a password.
