# Mobile-Heartrate-Monitor

It's a mobile heartrate monitor on AT32F403A-START develop board with RT-Thread.

The RT-Thread version I'm using is RT-Thread v4.0.2.

It works with an Air720H module with AT interface and a Max30102 heartrate sensor.

The Air720H connects to AT32 via PA2 as UART_RX and PA3 as UART_TX.

Max30102 sensor connects to AT32 via PB6 as SCL, PB7 as SDA and PC6 as INT line.

The host ip address and port is at 51th line of "dupload.c" in the driver folder.

It will send heartrate data to host with a data structure takes 4 bytes.

The first byte is frame type, 0 means data and 255 means terminate host access.

The second byte is device number. In this application, it's 1.

The third byte is heart rate.

The fourth byte is the checksum byte, which equals byte0 xor byte1 xor byte2.
