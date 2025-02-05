FreeRTOS Application in a Real-Time Air Quality Monitoring Architecture for Smart Campus.
This project employs FreeRTOS, a real-time operating system (RTOS), within the scenario of a smart university campus. The EndNode with RTOS was compared to an EndNode without RTOS. The main objective of this project is to improve detection time, transfer rate, transmission and reception time, and time gain in an air quality monitoring architecture.  The following are the main steps used to model our project and details on how to implement it.

# 1.	Server</h1>
<p>The LoRaWAN server is the machine responsible for receiving all information from the EndNodes. The machine has three necessary programs, which are:</p>
<li>A	Docker as the main program to abstract the processes that receive the data.</li>
<li>B.	The ChirpStack middleware, which is responsible for providing the necessary credentials that will be passed to the EndNodes.
(Refer to the documentation: https://www.chirpstack.io/docs/). </li>
<li>C.	FIWARE to send, decode, forward, and store data in a database.
(Documentation: https://github.com/alife-echo/iot-fiware). </li>
<p></p>
<b>Note: a Gateway is required to receive the LoRaWAN signal.</b>

<h1>2.	EndNodes</h1>
The EndNodes are two Heltec WiFi LoRa 32 (V2). Each of the EndNodes is connected to MQ131 and MQ135 sensors and a microSD card module.
<h2>2.1	How to configure LoRaWan Keys</h2>

<h2>2.1.1	EndNode without RTOS</h2>
Note that in the code “Monitoramento_da_Qualidade_do_Ar.ino” inside the Sem RTOS folder, there are the lines:</br><br>
static const PROGMEM u1_t NETWORKSKEY[16] = {0xCC, 0x19, 0xBD, 0xBC, 0x0E, 0xEC, 0xF2, 0x01, 0x2A, 0xC4, 0xC4, 0x6C, 0x3E, 0xFF, 0xE7, 0x2C };<br><br>
static const u1_t PROGMEM APPSKEY[16] = {0xB6, 0x3B, 0x24, 0x99, 0xED, 0x7E, 0x11, 0x5A, 0xF0, 0x76, 0x3D, 0x67, 0x7A, 0x67, 0x0E, 0x56};<br><br>
static const u4_t DEVADDR = 0x01278d0b;<br><br>
These lines are respectively: the Network Key, the Aplicative Keys, and the development address. Such keys will be obtained from ChirpStack.<br><br>

<h3>2.1.2	EndNode with RTOS</h3>
Among the many headers in Performance_com_RTOS, there is a header called chaves (keys). Inside this header, there are the same codes mentioned above, which serve the same purpose.
Note: the keys of End Node without RTOS and EndNote as RTOS will be different, each key abstracts a sensor in the chirpstack.

<h2>2.2	Libraries used:</h2>
<p>Some libraries are common within the IDE (we used the Arduino IDE to develop the project). 
The libraries that were used are:</p>
<li>MQUnifiedSenso - library responsible for configuring the sensors (See Documentation: https://github.com/miguel5612/MQSensorsLib).</li>
<li>SDCard - library responsible for detecting and reading the SD card (See Documentation: https://docs.arduino.cc/libraries/sd/).</li>
<li>WiFi - responsible for connecting the EndNodes to the internet to synchronize the clocks (See Documentation: https://docs.arduino.cc/libraries/wifi/).</li>
<li>MCCI Arduino LoRaWan - library responsible for LoRaWan configuration and connection (See Documentation: https://github.com/mcci-catena/arduino-lorawan).</li>
<li>NTPClient - library responsible for synchronizing the logical clocks of the EndNodes. The repository contains both files necessary for implementation.</li><br>
<b>To install the libraries, it is recommended to download all files in .zip format.</b><br>
For instructions, see documentation: https://github.com/arduino-libraries/NTPClient.<br>
<p>In the library in the repository there are the following modifications:
<li>a.	String NTPClient::getExtendDate() <br>
The method is based on the getDay method which returns an integer, this integer represents the days of the week, this integer value is stored to be used as a conditional so that the library in question knows which day of the week it is at the moment.</li>
<li>b.	String NTPClient::getFormattedDate(unsigned long secs, int choose)<br>
This method allows the to choose the date return format.</li>
<li>c.	String NTPClient::getFormattedTimeMillis(unsigned long secs)<br>
This method returns times formatted with milliseconds.</li></p>

<h1> 3.	Location of Codes</h1>
Inside the “códigos” folder there are the following files:
<li>a.	In the folder Biblioteca_Comum resides the NTPClient Library, the library was modified to meet the needs of the project, it needs to be in .zip to be installed within the Arduino IDE.</li>
<li>b.	In the folder Codigos_Gráficos are all the codes that were used to plot the graphs.</li>
<li>c.	In the folder ComRTOS has the main code that was used as an experiment and compiled for Heltec Wifi LoRa 32 (v2). Here it was using super loop programming. </li>
<li>d.	In the folder SemRTOS has the code that was used as an experiment and compiled for Heltec Wifi LoRa 32 (v2). Here it was using RTOS programming.</li>
<h1>4. From the Tables </h1>
<p> In the Data folder, there are the Spreadsheets used to analyze the scenarios, each Spreadsheet is a different scenario.
Within these files there will be five tables, the first two are the data collected from the EndNodes without RTOS TX and RX (Transmission and Reception), then the data collected from the EndNodes with RTOS in TX and RX, finally the comparison table, where we compare the time difference of the EndNodes. </p>
<p> In the Margin of Error Spreadsheet there is the data that was extracted from each scenario, this data is present in the spreadsheets referring to the scenarios. </p>
<p> The Percentage of Data Sends and Failures Spreadsheet is the total number of transmissions made and the total number of receptions made and the percentage of data lost. 
<br><br><b></b>
Note: Spreadsheets are in .rar format to protect against corruption, download and extract as needed</p>