# OLED-gauge

[URL=http://s160.photobucket.com/user/dan22h/media/B454BA7A-D0DD-42E0-9541-96540D9E4632_zps7xrowrfv.jpg.html][IMG]http://i160.photobucket.com/albums/t169/dan22h/B454BA7A-D0DD-42E0-9541-96540D9E4632_zps7xrowrfv.jpg[/IMG][/URL]

Code to drive a 4d systems uOLED 160 display module using an attiny84 or arduino for a car/automotive gauge. Sample images for Oil pressure, oil temp, voltage etc
A project I created to implement an oled display (4d systems uOLED 160) as an automotive gauge display for my Lotus Elise. The gauges run of an attiny84 with dedicated PCB containing built in power supply with circuit protection or an arduino connected to appropriate sensors.

To install, you will need to download the 4D workshop IDE from the 4d systems website. Now navigate to this folder "C:\Users\Public\Documents\4D Labs\4DUpdates\Utils" and open the "uoled-160-g2.inc" text file in notepad, replace the contents with the following to enable splash screen on startup;

media_Init();  
media_VideoFrame(0, 0, 1);

Now open the 4d workshop program, from the options under 'serial' set the baud rate to 38400 and disbale the screensaver by entering 0. Start a new serial project for the uOLED 160 display, once created select tools then SPE load, this will allow the arduino to communicate with the display and will also load the code to run the splash screen on startup.

Start the 4d systems graphics composer program and open the .gcs file you wish to run, this will load the gif file, now select build, choose the uSD card you wish to burn and select OK, the SD card is now ready to use.

Upload the arduino file that matches the .gcs file used to either an arduino or attiny84 (select your own pins if appropriate). The arduino is setup to use an LMT86 analogue temperature sensor for it's high accuracy fitted to an automotive temp sensor housing. the default pressure sensor is a digital 0-100psi sensor chosen for it's precision linear output. other sensors can be used by simply altering the code on the arduino.

Simply connect the arduino to the display and appropriate sensors then apply power to the assembly.

