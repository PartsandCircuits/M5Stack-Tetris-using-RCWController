# M5Stack-Tetris-using-RCWController

<br />

<p align="center">
 
<img src="https://github.com/PartsandCircuits/M5Stack-Tetris-using-RCWController/blob/master/Tetris_RCWController.png" width="700">

</p>


<br />

## About
- Use an iPhone based virtual gamepad - RCWController - to control and play Tetris on an M5Stack via a WiFi connection.


- [Video demonstration](https://www.youtube.com/watch?v=jeuHP-NbdJg&feature=youtu.be) - this also includes a link to the original source of the code provided here.


- Details about the RCWController app are outlined [here](https://appadvice.com/app/rcwcontroller/1084628679)

<br />

## Requirements
- Any M5Stack model
- an iPhone
- Arduino IDE
- Tested working with M5Stack library ver 0.2.0


<br />

## Setup
1. Clone or download code above, open in Arduino IDE. 
2. Edit code - add your WiFi details - SSID and password - to the code. Save & upload to M5Stack.  
3. Grab an iPhone. Go to the Apple store. Search for RCWController. Install.
4. Turn M5Stack on. It will try to establish a WiFi connection. The IP address being used will be displayed on the M5Stack screen.
5. Open the RCWController app. Note the green button in the middle of the screen between the "Select" and the "Start" button. Press twice & the IP address setup screen should appear. Enter the IP address number being used by the M5Stack. Press the "Connect" button on the bottom right of the screen.

<br />


## Controls

- To start the game, press the "B" button on the RCWController.  
- To play a new game, restart the M5Stack & press the "B" button.

- Gamepad  =  Up, down, left, right
- A or B button  =  rotate Tetris piece  

<br />


## Credits

- RCWController app by Micono Utilities 

- RCWController support for Tetris on the M5Stack by Micono Utilities

- Tetris ported to M5Stack by Macsbug - https://macsbug.wordpress.com/

- Tetris originally from MhageGH - https://github.com/MhageGH/esp32_ST7735_Tetris


