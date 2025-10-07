# Everything you need to know to construct the analog front end (ALL THE HARDWARE)


## Microcontroller Board
First and foremost, here's the board I used. https://www.amazon.com/dp/B09MLGWBQ2?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1. It's cheap and you get 3 for a bit over the price of 1 on other sites. Now with that being said, you also need an ST Link V2 Programmer since it doesn't come preflashed. It isn't the end of the world since you can really just tape down four pieces of electric tape with the pins under (1 for SWDIO, 1 for SWCLK, and 1 for GND and 3.3V each) anyway and flash the board.

## Accessories
**All the accessories I used**

Next thing is, having perfboards to build this on is awesome since you're soldering. I bought these https://www.amazon.com/dp/B0DBZ1BXFZ?ref=ppx_yo2ov_dt_b_fed_asin_title and they come in a bunch of sizes so you can use them for more projects.

To get the actual guitar jack to plug in, buy these: https://www.amazon.com/dp/B07P77KZ3V?ref=ppx_yo2ov_dt_b_fed_asin_title. They're stereo and you only need mono so just solder to the respective prong that matches the one closer to the tip of the plug (if you actually see one of these, you'll understand what I mean, it isn't too bad at all).

https://www.amazon.com/dp/B072Q2X2LL?ref=ppx_yo2ov_dt_b_fed_asin_title is a great and cheap OLED to buy. It's a pack of 1, (all you need for this project), and you since it's the only SPI, it doesn't require too much work to make it work anyway.

Buttons: I had a small arduino set I got gifted by a family member so I didn't buy buttons (I'm using my arduino for something else so it wasn't available to be used here) but buttons are cheap on Mouser or Amazon.

Wires: Refer to buttons

## Hardware for Input
**Your next order of business is actual hardware.**
You need 2x 100k Ohm Resistors, 1x 10k Ohm Resistors, 1x 1.0uF capacitor (ceramic is better since it's not strict about direction (not polar)), 1x 4.7nF capacitor, 1x 576-SMF5.0A ESD Protection Diode, and Solder wire.

Your path should look like this... I'll make it in simulink sometime soon, along with posting a video of it actually helping tune. 
<img width="729" height="591" alt="image" src="https://github.com/user-attachments/assets/fb11dda4-5217-457b-8335-92819738b522" />

From the guitar tip, the cathode of the ESD Protection device should be connected, then the anode goes to ground. From that same node, you should also put the 1uF (coupling) capacitor. From there, it hits a 10k Ohm resistor in series. At this point we'll call it Node B. This node essentially has 4 things in parallel. 2x 100k ohm resistors, one that hits 3v3 and one that hits GND. Next you have a 4.7 (decoupling) capacitor, which also goes to GND, finally, you have your wire that takes you to (in my case) PB1, but you can just check which GPIO actually supports ADC and use any.

The rest of the OLED and buttons is really just soldering to the right input and to GND and 3V3.

An actual walkthrough of the perfboard and simulink will probably be made soon...
