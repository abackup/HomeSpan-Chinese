# HomeSpan Status and the HomeSpan Status LED

In addition to keeping track of all the HomeKit Accessories, Services, and Characteristics you've implemented in a HomeSpan sketch, HomeSpan also keeps track of its global operating status (e.g. connecting to WiFi, updating via OTA, etc.).  The current HomeSpan status, represented as an *enum* of type **HS_STATUS**, can be read directly from within your sketch as well as communicated visually via different blinking patterns of an *optional* Status LED you can implement on your device.  This LED can be a simple analog single-color LED or an addressable Pixel LED. Many manufacturers include one or both of these on their ESP32 boards, though you can use a separate LED if desired.

To enable HomeSpan's Status LED functionality, simply call `homeSpan.setStatusPin()` or any related functions near the top of your HomeSpan sketch depending on the specific type of LED you are using.  Please see the [Reference API](Reference.md) for details on the various options available for enabling different types of Status LEDs.

To instead read the status from within your sketch, call `homeSpan.getStatus()`.  This function returns a *pair* of values in the form `std::pair<HS_STATUS, uint32_t>`, where the first element indicates HomeSpan's current *status* and the second element indicates the *duration* (in seconds) since HomeSpan entered into that state.  This duration resets to zero whenever the HomeSpan status changes, and can also be reset manually by calling `homeSpan.resetStatusDuration()`.

For example, adding the following code to the main `loop()` in your sketch will monitor HomeSpan's status and output a warning every 30 seconds if the device has not yet been paired:

```C++
void loop(){

  homeSpan.poll();

  auto [ status, duration] = homeSpan.getStatus();        // using "auto" is an easy way to read the data from a function that returns a std::pair
  if(status==HS_PAIRING_NEEDED && duration > 30){         // check that current status has been HS_PAIRING_NEEDED for more than 30 second  
    Serial.printf("Warning: HomeSpan is not paired.\n");  // print warning message
    homeSpan.resetStatusDuration();                       // re-start the status duration timer
  }  
}
```

In addition to reading the HomeSpan status directly, the method `homeSpan.setStatusCallback(void (*func)(HS_STATUS status))` can be used to create an optional callback function, *func*, that HomeSpan calls whenever it changes its status.[^setup]  HomeSpan passes *func* a single argument, *status*, of type **HS_STATUS**, that you can read from within your callback.  For ease of use, HomeSpan also provides the convenience function `const char* homeSpan.statusString(HS_STATUS s)` that returns a pre-defined character string for each **HS_STATUS** type.

[^setup]: Note HomeSpan boots with its status initialized to *HS_INITIAL_SETUP*, which itself does *not* trigger the callback.  The callback won't be triggered until  HomeSpan first changes its initial status from *HS_INITIAL_SETUP* to something else, and then upon all other subsequent changes.

For example, the code below creates an optional callback function printing the current HomeSpan status whenever it changes:


```C++
#include "HomeSpan.h"

void setup(){
  homeSpan.setStatusCallback(statusUpdate);   // set callback function
  ...
  homeSpan.begin();
  ...
}

// create a callback function that simply prints the pre-defined character string on the Serial Monitor whenever the HomeSpan status changes

void statusUpdate(HS_STATUS status){
  Serial.printf("\n*** HOMESPAN STATUS CHANGE: %s\n",homeSpan.statusString(status));
}
```

You can of course create any alternative messsages, or take any actions desired, in *func* and do not need to use HomeSpan's pre-defined character strings.  Note that HomeSpan callbacks can also take anonymous *lambda functions* as an argument.  For example, this single line of code will create a callback that outputs log messages, formatted in red, to the Web Log whenever the HomeSpan status changes:

```C++
void setup(){
  ...
  homeSpan.setStatusCallback([](HS_STATUS status){WEBLOG("<span style\=\"color:red\">HOMESPAN STATUS: %s</span>",homeSpan.statusString(status));});
  ...
}
```

### Table of HomeSpan Status Types

The table below lists all possible HomeSpan **HS_STATUS** types along with each type's pre-defined character string (as returned by `statusString()`) and a graphic representation of the flashing pattern HomeSpan would display for that status type on the optional HomeSpan Status LED, if enabled.  Click on any entry for more details.  Note that graphic representations are all scaled to show the first 6 seconds of each pattern, which should be always sufficient to make the pattern (number of blinks, duration, etc.) obvious.

|HomeSpan Status (HS_STATUS)|Status String|Status LED Pattern|
|---|---|---|
|<details><summary>HS_INITIAL_SETUP</summary><i>HomeSpan is running code in the setup() portion of the sketch</details>|HomeSpan Initializing|<img src="images/ledPatterns/off.svg" width=300>|
|<details><summary>HS_WIFI_NEEDED</summary><i>WiFi Credentials have not yet been set/stored, and an Ethernet interface is not available</details>|WiFi Credentials Needed|<img src="images/ledPatterns/slowSingleBlink.svg" width=300>|
|<details><summary>HS_WIFI_SCANNING</summary><i>HomeSpan is in the process of scanning (or re-scanning) for WiFi network Access Points</details>|WiFi Scanning Started|<img src="images/ledPatterns/longTripleBlink.svg" width=300>|
|<details><summary>HS_WIFI_CONNECTING</summary><i>HomeSpan is trying to connect to the WiFi network specified in the stored WiFi Credentials</details>|WiFi Connecting|<img src="images/ledPatterns/slowFlashing.svg" width=300>|
|<details><summary>HS_ETH_CONNECTING</summary><i>HomeSpan is trying to connect to an Ethernet network using the Ethernet interface configured</details>|Ethernet Connecting|<img src="images/ledPatterns/slowFlashing.svg" width=300>|
|<details><summary>HS_PAIRING_NEEDED</summary><i>HomeSpan is connected to a network, but the device has not yet been paired with HomeKit</details>|Device not yet Paired|<img src="images/ledPatterns/slowDoubleBlink.svg" width=300>|
|<details><summary>HS_PAIRED</summary><i>HomeSpan is connected to a network and the device has been paired with HomeKit, but there are no active HomeKit connections</details>|Paired and waiting for HomeKit|<img src="images/ledPatterns/slowDoubleBlinkInverted.svg" width=300>|
|<details><summary>HS_CONNECTED</summary><i>HomeSpan is connected to a network, the device has been paired to HomeKit, and there is at least one active HomeKit connection</details>|Device is Connected to HomeKit|<img src="images/ledPatterns/on.svg" width=300>|
|<details><summary>HS_ENTERING_CONFIG_MODE</summary><i>User has requested the device to enter into Command Mode</details>|Entering Command Mode|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_EXIT</summary><i>HomeSpan is in Command Mode with "Exit Command Mode" specified as choice</details>|1. Exit Command Mode|<img src="images/ledPatterns/fastBlink1.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_REBOOT</summary><i>HomeSpan is in Command Mode with "Reboot" specified as choice</details>|2. Reboot Device|<img src="images/ledPatterns/fastBlink2.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_LAUNCH_AP</summary><i>HomeSpan is in Command Mode with "Launch Access Point" specified as choice</details>|3. Launch Access Point|<img src="images/ledPatterns/fastBlink3.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_UNPAIR</summary><i>HomeSpan is in Command Mode with "Unpair Device" specified as choice</details>|4. Unpair Device|<img src="images/ledPatterns/fastBlink4.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_ERASE_WIFI</summary><i>HomeSpan is in Command Mode with "Erase WiFi Credentials" specified as choice</details>|5. Erase WiFi Credentials|<img src="images/ledPatterns/fastBlink5.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_EXIT_SELECTED</summary><i>User has selected "Exit Command Mode"</details>|Exiting Command Mode...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_REBOOT_SELECTED</summary><i>User has select "Reboot" from the Command Mode</details>|Rebooting Device...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_LAUNCH_AP_SELECTED</summary><i>User has selected "Launch AP Access" from the Command Mode</details>|Launching Access Point...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_UNPAIR_SELECTED</summary><i>User has selected "Unpair Device" from the Command Mode</details>|Unpairing Device...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_ERASE_WIFI_SELECTED</summary><i>User has selected "Erase WiFi Credentials" from the Command Mode</details>|Erasing WiFi Credentials...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_REBOOTING</summary><i>HomeSpan is in the process of rebooting the device</details>|REBOOTING|<img src="images/ledPatterns/off.svg" width=300>|
|<details><summary>HS_FACTORY_RESET</summary><i>HomeSpan is in the process of performing a Factory Reset of device</details>|Performing Factory Reset...|<img src="images/ledPatterns/off.svg" width=300>|
|<details><summary>HS_AP_STARTED</summary><i>The HomeSpan Setup Access Point is started but no client has yet connected</details>|Access Point Started|<img src="images/ledPatterns/rapidDoubleBlink.svg" width=300>|
|<details><summary>HS_AP_CONNECTED</summary><i>The HomeSpan Setup Access Point is running and a client is connected</details>|Access Point Connected|<img src="images/ledPatterns/mediumDoubleBlink.svg" width=300>|
|<details><summary>HS_AP_TERMINATED</summary><i>The HomeSpan Setup Access Point has been terminated</details>|Access Point Terminated|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_OTA_STARTED</summary><i>HomeSpan is in the process of receiving an Over-the-Air software update</details>|OTA Update Started|<img src="images/ledPatterns/mediumTripleBlink.svg" width=300>|

### Advanced Use Cases

* If you have attached a text display to your HomeSpan device, add code to the callback function decribed above to have it display HomeSpan status messages to the user.
* Instead of enabling the HomeSpan Status LED, implement your own custom LED patterns, use different colors, etc., by adding custom logic to the callback function.

### Rebooting to Address "No Response" Issues in Home App

After a HomeSpan device establishes WiFi or Ethernet connectivity to your network, its status will change to either *HS_PAIRING_NEEDED*, or *HS_PAIRED*, depending on whether or not it has been paired with HomeKit.  If the status is *HS_PAIRING_NEEDED*, it will remain so until you pair the device with HomeKit using the Home App on your iPhone.

If instead pairing has already been completed, one or more of your HomeKit Hubs should automatically detect the HomeSpan device and initiate a secure connection request.  This may occur within a few seconds of the device first connecting to your network, or it may take a minute or two.  Upon successfully validating the first request HomeSpan receives for a secure connection from HomeKit, HomeSpan changes its status to *HS_CONNECTED*.  While in that state, you should be able to fully control your device from the Home App.

If for whatever reason HomeKit closes all its secure connections with the device, but the device remains connected to your network and paired with HomeKit, HomeSpan will reset its status to *HS_PAIRED*. It then waits for HomeKit to request a new secure connection.

As such, HomeSpan should maintain a status of *HS_PAIRED* only temporarily.  Remaining in the *HS_PAIRED* state for an extended period of time is undesireable and will likely cause the Home App to indicate "No Reponse" for your device.  HomeSpan cannot cure this issue by itself, since HomeKit does not allow devices to initiate outbound connections to a HomeKit Hub.  Rather, HomeKit must initiate all new connections to each device it finds on a home network.

The code below provides a potential solution to this problem by monitoring HomeSpan's status and checking if it has been in the *HS_PAIRED* status for an extended period of time. If so, it assumes something is wrong and reboots itself, with the hope that upon re-starting and re-connecting to your network, HomeKit will re-establish secure connectivity with the device:

```C++
void loop(){

  homeSpan.poll();

  auto [ status, duration] = homeSpan.getStatus();        // using "auto" is an easy way to read the data from a function that returns a std::pair
  if(status==HS_PAIRED && duration > 120)                 // check if HomeSpan has been waiting more than 2 minutes for a HomeKit connection
    homeSpan.processSerialCommand("R");                   // if so, force HomeSpan to reboot
}
```

---

[↩️](../README.md) Back to the Welcome page
