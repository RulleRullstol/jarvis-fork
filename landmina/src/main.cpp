#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <vector>

// Connection pins for INMP441 on ESP32
#define I2S_WS 25                     // Word Select pin
#define I2S_SD 33                     // Serial Data pin
#define I2S_SCK 32                    // Serial Clock pin

// I2S audio settings
#define I2S_PORT I2S_NUM_0            // Use I2S Processor 0
#define BUFFER_SIZE 256               // I2S buffer, max 256
#define SAMPLE_RATE 22050          	  // Sample rate for I2S
uint8_t audioData[BUFFER_SIZE];       // I2S buffer
int16_t sBuffer[BUFFER_SIZE / 2];     // I2S samples (16-bit per sample)

// Network settings
const char *ssid = "NG24";            // WiFi SSID
const char *password = "123asdqwe";   // WiFi Password
//const char* ssid = "TN_wifi_D737B5_EXT";
//const char* password = "LDMAEJJWDU";

// UDP settings
WiFiUDP udp;
const int remotePort = 10000;         // Data port
const int broadcastPort = 9999;       // Broadcast port
const String ackMsg = "OK ESP_0";     // Acknowledgment message from Pi
String msg = "Hej hej :) ESP_0 10000";// Unique message for each ESP, 10000 is the port for data
IPAddress remoteIP;                   // Pi's IP address

// Semaphore & udp buffer
SemaphoreHandle_t xSemaphore = NULL; // Semaphore for task synchronization
const int UDP_BUFFER_SIZE = 2048;    // This impacts the size needed for the task
const int UDP_PACKET_SIZE = 1024;    // Keep under 1400 otherwise udp.write() arg
std::vector<uint8_t> udpBuffer;

void i2s_install() {
	// Set up I2S Processor configuration
	const i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
		.sample_rate = SAMPLE_RATE,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 16-bit samples
		.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = 0,
		.dma_buf_count = 8,
		.dma_buf_len = BUFFER_SIZE / 2,  // Number of samples in the buffer
		.use_apll = false};

	i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
	// Set I2S pin configuration
	const i2s_pin_config_t pin_config = {
		.bck_io_num = I2S_SCK,
		.ws_io_num = I2S_WS,
		.data_out_num = -1,
		.data_in_num = I2S_SD};

	i2s_set_pin(I2S_PORT, &pin_config);
}

void broadcast(WiFiUDP &udp) {
	char buffer[255];
	String rcvdMsg;

	Serial.println("Broadcasting to IP: " + WiFi.broadcastIP().toString());
	udp.begin(broadcastPort);
	delay(50);
	while (true)
	{
		Serial.println("Sending UDP broadcast...");
		udp.beginPacket(WiFi.broadcastIP(), broadcastPort);
		udp.write((uint8_t *)msg.c_str(), msg.length());
		delay(1);
		udp.endPacket();
		delay(500); // Give time for response

		int packetSize = udp.parsePacket(); // Greppa packet
		Serial.printf("Packet Size Received: %d\n", packetSize);

		if (packetSize)
		{
			int readBytes = udp.read(buffer, sizeof(buffer) - 1);
			if (readBytes > 0)
			{
				buffer[readBytes] = '\0';
				rcvdMsg = String(buffer);
				rcvdMsg.trim(); // Bort med skumma grejer
			}

			Serial.println("Received message: " + rcvdMsg);

			if (rcvdMsg == ackMsg)
			{
				remoteIP = udp.remoteIP();
				Serial.println("Acknowledgment received!");
				delay(500);
				udp.stop();
				return;
			}
		}
		else
		{
			Serial.println("Retrying..");
		}
	}
}

// Send audio data over UDP
void udpSend(void *pvParameters) {
    std::vector<uint8_t> tempData;
	uint lastTimeSent = 0;
    while (true) {
        // Wait for semaphore
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
            // Can packet be sent?
            if (udpBuffer.size() >= UDP_PACKET_SIZE) {
                tempData.insert(tempData.end(), udpBuffer.begin(), udpBuffer.begin() + UDP_PACKET_SIZE);
                udpBuffer.erase(udpBuffer.begin(), udpBuffer.begin() + UDP_PACKET_SIZE);
                udpBuffer.shrink_to_fit(); // Fixa minnesläcka
                uint8_t* data = tempData.data();  // Convert to array
                Serial.printf("Sent packet of size: %zu bytes\n", UDP_PACKET_SIZE);  // Debug

                udp.beginPacket(remoteIP, remotePort);
                udp.write(data, UDP_PACKET_SIZE);
                udp.endPacket();
				Serial.printf("Time since last packet: %d\n", (micros() - lastTimeSent) / 1000);
				lastTimeSent = micros();
                tempData.clear();
            }
        }
        vTaskDelay(1); // Tydligen viktigt när det gäller freertos tasks
    }
}

void setup() {
	Serial.begin(250000);
	// Connect to WiFi
	Serial.println("Connecting to WiFi...");
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
	broadcast(udp);		   // Get remote ip & port
	udp.begin(remotePort); // Data port
	delay(100);			   // Wait for udp
	Serial.println("Sending data to: " + remoteIP.toString() + ":" + remotePort);

	// semaphore for udpSend sync
	xSemaphore = xSemaphoreCreateBinary();

	// create freertos task for udpSend
	xTaskCreatePinnedToCore(
		udpSend,                    // Task function
		"udpSend",                  // Task name
		16384,                      // Stack size
		NULL,                       // Parameters
		1,                          // Priority
		NULL,                       // Task handle
		1);                         // Core (0 or 1)
}

void loop() {
    // Read I2S data into the sBuffer
    size_t bytesIn = 0;
    i2s_read(I2S_PORT, &sBuffer, BUFFER_SIZE / 2 * sizeof(int16_t), &bytesIn, portMAX_DELAY);

    // Convert I2S data (16-bit samples) into a byte array and copy into audioData
    for (size_t i = 0; i < BUFFER_SIZE / 2; i++) {
        audioData[i * 2] = (uint8_t)(sBuffer[i] & 0xFF);            // Low byte
        audioData[i * 2 + 1] = (uint8_t)((sBuffer[i] >> 8) & 0xFF); // High byte
    }

    // Prevent overflows
    if (udpBuffer.size() + BUFFER_SIZE <= UDP_BUFFER_SIZE) {
        udpBuffer.insert(udpBuffer.end(), std::begin(audioData), std::end(audioData));
    } else {
        Serial.println("Buffer full, dropping samples...");
    }
    // updSend go
        xSemaphoreGive(xSemaphore);
}