# Serial-mobile

This project is based on the MSP430g2553 chip, the TI Launchpad and the Energia IDE. This is a fork from the project [EspruinoOrion](https://github.com/espruino/EspruinoOrion). The same circuit has worked well for the msp430g2553 and I have successfully connected a cellphone to the TI launchpad.

The Energia project file contains a firmware that will enable the TI microcontroller to receive commands via the UART interface. It currently supports read/write i2c, set and write pins outputs and read pins as inputs. 

In the Web folder, there is a client and a server files. Server file is supposed to be connected to the microcontroller through the audio jack. It encodes the commands to be received with the UART interface. The Server creates a [bugout](https://github.com/chr15m/bugout) server, which uses webtorrent to allow p2p connections via WebRTC. The Client server can connect to the server either by using the address generated by the server or by using a camera o scan the QR code shown in the server device screen. 