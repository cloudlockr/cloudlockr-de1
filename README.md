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

tests.c also contains AES and HEX tests, and several tests for some controller messages. Simply uncomment the main function at the end and comment out the main function in cloudlockrMain.c to verify that those functions have the correct result.
 
## Directory Structure
/AES: Verilog+Python code for hardware accelerated AES encryption/decryption and verification.

/CPEN391FW: Includes C code for UART interfacing of RFS board with DE1, and JSON module for handling Bluetooth and Wi-fi messages. 

/Cloudlockr_Computer: The main Quartus project folder for programming the DE1-SoC Board.


## How to run DE1 Firmware

1. Open Quartus Project, use Programmer to load .sof onto DE1
2. Use OpenConnect to connect to UBC ECE license. 
3. Run "eclipse &" from EDS command shell as administrator.
4. Check that the scatter file for the project is correct, as the path is not relative to the workspace or project.
5. Download preloader by running command below in SoC EDS 15.0 Command Shell:
C:/Altera/15.0/quartus/bin64/quartus_hps --cable="DE-SoC [USB-1]" -o GDBSERVER --gdbport0=3335 --preloader=C:/Altera/15.0/University_Program/Monitor_Program/arm_tools/u-boot-spl.srec --preloaderaddr=0xffff1398
(If you are using SoC EDS 19.1, use --preloader=C:/intelFPGA_lite/19.1/University_Program/Monitor_Program/arm_tools/u-boot-spl.de1-soc.srec --preloaderaddr=0xffff13a0)
6. When "Starting GDB Server." shows up, press Ctrl-C to let DS5 start GDB server by itself.
7. In order to reset the Wi-fi component before running the C code in DS5, press down on KEY 0 on the DE1.
8. In DS5, Build Project, then do Debug As->Debug Configurations, select CPEN391_Config (as created in the tutorial) and press Debug.
9. Press "Run" (green arrow button).

notes: 
- above preloader command can be run before starting eclipse in instruction 3.
- wifi network and password needs to be set in WIFI_Init function in WIFI.c
- EDS = Altera Embedded Command Shell
