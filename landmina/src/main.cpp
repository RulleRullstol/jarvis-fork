
// Include I2S driver & WIFI
#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Connection pins ti INMP441 on ESP32
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
 
// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0
 
// Define input buffer length
#define bufferLen 256
int16_t sBuffer[bufferLen];

// WIFI settings
const char* ssid = "NG24";
const char* password = "123asdqwe";

// UDP settings
const int remotePort = 10000;
const int broadcastPort = 10000;	// Port för broadcast, skickar denna 
const String ackMsg = "OK";			// Meddelande från pi

String msg = "Hej hej :) Jag är ESP_1 10000"; // Meddelande, unik för varje ESP, 10000 är port som data ska skicar över
uint8_t remoteIP; // Pi IP


//const char* remoteIP = "192.168.10.100"; // IP of Raspberry Pi

void broadcast(WiFiUDP udp) {
	char buffer[255];
	int senderIp;
	String rcvdMsg;
	Serial.println("Broadcast started");
	while(true) {
		udp.beginPacket(WiFi.broadcastIP(), broadcastPort);
		udp.write((uint8_t*)msg.c_str(), strlen(msg.c_str()));
		//Serial.print("Sending packet: " + msg + " To: " + WiFi.broadcastIP() + ":" + broadcastPort);
		udp.endPacket();
		udp.begin(broadcastPort);

		int packetSize = udp.parsePacket();
		if(packetSize) {
			Serial.println("Message recieved...");
			int readBytes = udp.read(buffer, sizeof(buffer) -1);
			if (readBytes > 0) {
			buffer[readBytes] = '\0'; // Konstig null-termination
			remoteIP = udp.remoteIP();
			}
			rcvdMsg = String(buffer);
			Serial.print("Stringified message: " + rcvdMsg);
			if(rcvdMsg == ackMsg) {
				delay(2000);
				return;
			}
		}
	}
}

void i2s_install() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };
  
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };
  
  i2s_set_pin(I2S_PORT, &pin_config);
}
WiFiUDP udp;
void setup() {
  
  // Set up Serial Monitor
  Serial.begin(250000);
  Serial.println(" ");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  delay(1000);
  
  // Set up I2S
  i2s_install();
  i2s_setpin();
  	i2s_start(I2S_PORT);
  
  // Init UDP & broadcast & get pi
  broadcast(udp);
  
  	delay(500);
}

void loop() {
  
  	// False print statements to "lock range" on serial plotter display
  	// Change rangelimit value to adjust "sensitivity"
 
  	// Get I2S data and place in data buffer
  	size_t bytesIn = 0;
  	i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
 
  	// Read I2S data buffer
  	uint8_t audioData[bufferLen * 2];  // Each 16-bit sample will take 2 bytes

// Loop through each sample in sBuffer and convert to bytes
  	for (size_t i = 0; i < bufferLen; i++) {
    	// sBuffer[i] is a 16-bit sample, so break it into 2 bytes
    	audioData[i * 2] = (uint8_t)(sBuffer[i] & 0xFF);        // Low byte
    	audioData[i * 2 + 1] = (uint8_t)((sBuffer[i] >> 8) & 0xFF);  // High byte
 	 }

  	udp.beginPacket(remoteIP, remotePort);
  	udp.write(audioData, sizeof(audioData));
  	udp.endPacket();

 	 delay(50);
}