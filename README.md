# cloudlockr-de1

Firmware for Cloudlockr. Includes C code for UART interfacing of RFS board with DE1, and Verilog+Python code for hardware accelerated AES encryption/decryption and verification

# How to run DE1 Firmware

1. Open Quartus Project, use Programmer to load .sof onto DE1
2. Use OpenConnect to connect to UBC ECE license. 
3. Run "eclipse &" from EDS command shell as administrator.

4. Download preloader by running command below in SoC EDS 15.0 Command Shell:
C:/Altera/15.0/quartus/bin64/quartus_hps --cable="DE-SoC [USB-1]" -o GDBSERVER --gdbport0=3335 --preloader=C:/Altera/15.0/University_Program/Monitor_Program/arm_tools/u-boot-spl.srec --preloaderaddr=0xffff1398

5. When "Starting GDB Server." shows up, press Ctrl-C to let DS5 start GDB server by itself.

6. In order for Wi-fi to work (currently), before running the C code in DS5, use a jumper wire to touch the Wifi reset signal on the RFS to Ground. 
This means touching pin 2 to pin 12 which are the 1st and 6th metals pins from the top, when looking at the back of the RFS(with ribbon cable connector on right side).

7. In DS5, Build Project, then do Debug As->Debug Configurations, select CPEN391_Config and press Debug.
8. Press "Run" (green arrow button).

notes: 
- above preloader command can be run before starting eclipse in instruction 3.
- wifi network and password needs to be set in WIFI_Init function in WIFI.c
- EDS = Altera Embedded Command Shell

#How to use sample SPP app

First, go to SPP\app\src\main\java\com\example\spp\MainActivity.java 
to change:

private static String address

to whatever the MAC address of your own RFS Daughter Card's Bluetooth HC-05 module is. 

Use the "Bluetooth Pair" app from the Android play store to connect to the RFS when everything is connected and powered on. 
Bluetooth and Location services MUST be on. 
Press "Start Bluetooth Scan" to search nearby devices for HC-05 on the RFS, and the password to enter is "1234", which is the default for HC-05. 

When everything is set up, running this app on your own Android phone will send the string "Hello, Jason the great!!!\n"
