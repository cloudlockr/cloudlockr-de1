# cloudlockr-de1

This repo contains the Firmware for Cloudlockr. This codebase includes C code for UART interfacing of RFS board with DE1, and Verilog+Python code for hardware accelerated AES encryption/decryption and verification. The main Quartus project folder for programming the DE1-SoC Board is also found here.  

## Main Technologies

- C (Bluetooth, Wi-fi, Sensors, AES encryption/decryption)
- Python (AES encryption/decryption)
- Verilog (AES encryption/decryption, DE1-SoC programming)
- Quartus (DE1-SoC programming)
- DE1-SoC Board (physical hardware)
- RFS Daughter Card (physical hardware)
- Eclipse (running embedded software)

## Tests

For testing the gyroscope sensor, run the function MPU9250_CheckStationary() 
in cloudlockr-de1/CPEN391FW/source/mpu9250.c. This test will pass if the RFS Daughter Card is stationary 
or very close to being stationary. 

Similarly, for testing the magnetometer sensor, run the function MPU9250_CheckMagnetDirection(), 
which is also in cloudlockr-de1/CPEN391FW/source/mpu9250.c. This test will pass if the RFS Daughter Card is 
oriented towards the general Northern direction, which is used for passing a 32-bit value to AES encryption/decryption. 

These two sensor tests are run by default by getSensorKey(), which is used for AES encryption/decryption. 
Test results are printed in the Eclipse console. 
 
## Directory Structure
/AES: Verilog+Python code for hardware accelerated AES encryption/decryption and verification.

/CPEN391FW: Includes C code for UART interfacing of RFS board with DE1. 

/CPEN391_Computer (Verilog) UART - For 391 Students: The main Quartus project folder for programming the DE1-SoC Board.

/json-parsing: Contains JSON module for for handling Bluetooth and Wi-fi messages. 


## How to run DE1 Firmware

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

