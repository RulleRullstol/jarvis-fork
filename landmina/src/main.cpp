#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Connection pins for INMP441 on ESP32
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
//const char* ssid = "NG24";
//const char* password = "123asdqwe";

// UDP settings
const int remotePort = 10000;
const int broadcastPort = 9999; // Broadcast port
const String ackMsg = "OK ESP_0"; // Acknowledgment message from Pi

String msg = "Hej hej :) ESP_0 10000"; // Unique message for each ESP, 10000 is the port for data
IPAddress remoteIP; // Pi's IP address

WiFiUDP udp;

void broadcast(WiFiUDP& udp) {
	char buffer[255];
	String rcvdMsg;

	// Loop until Pi responds
	while (true) {
		udp.beginPacket(WiFi.broadcastIP(), broadcastPort);
		udp.write((uint8_t*)msg.c_str(), msg.length()); // Send the broadcast message
		//Serial.print("Sending packet: " + msg + " To: " + WiFi.broadcastIP().toString() + ":" + broadcastPort);
		delay(500);
		udp.endPacket();
		udp.begin(broadcastPort);
		delay(500); // Wait
		int packetSize = udp.parsePacket();
		Serial.println("packetSize__: " + (char)packetSize);
		if (packetSize) {
			int readBytes = udp.read(buffer, sizeof(buffer) - 1);
			if (readBytes > 0) {
				buffer[readBytes] = '\0'; // Null-terminate the string
				rcvdMsg = String(buffer);
				Serial.print(rcvdMsg);
			}

			Serial.print("Message received: " + rcvdMsg);
			if (rcvdMsg == ackMsg) {
				delay(2000);
				return; // Exit broadcast loop once acknowledgment is received
			}
		}
	}
}

void i2s_install() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
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

void setup() {
  // Set up Serial Monitor
  Serial.begin(250000);
  Serial.println("Connecting to WiFi...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

  delay(1000);

  // Set up I2S for audio capture
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  // Init UDP & broadcast to get Pi IP address
  broadcast(udp);
  delay(500); // Wait before starting audio transmission
}

void loop() {
  // Get I2S data and place in data buffer
  size_t bytesIn = 0;
  i2s_read(I2S_PORT, &sBuffer, bufferLen * sizeof(int16_t), &bytesIn, portMAX_DELAY);

  // Convert I2S data (16-bit samples) into a byte array
  uint8_t audioData[bufferLen * 2];  // Each 16-bit sample will take 2 bytes

  // Loop through each sample in sBuffer and break it into two 8-bit bytes
  for (size_t i = 0; i < bufferLen; i++) {
    audioData[i * 2] = (uint8_t)(sBuffer[i] & 0xFF);        // Low byte
    audioData[i * 2 + 1] = (uint8_t)((sBuffer[i] >> 8) & 0xFF);  // High byte
  }

  // Send the audio data over UDP
  udp.beginPacket(remoteIP, remotePort); // Use the IP address of the Raspberry Pi
  udp.write(audioData, sizeof(audioData)); // Send the byte array
  udp.endPacket(); // End the packet

  delay(50); // Adjust delay for data transmission rate
}