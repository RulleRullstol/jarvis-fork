import spidev
import time

# Initialize SPI
spi = spidev.SpiDev()
spi.open(10, 0)  # Use SPI bus 10, chip select 0 (CE0)
spi.max_speed_hz = 500000  # Lower clock speed for stability

# Function to read MCP3008 channel
def read_adc(channel):
    if channel < 0 or channel > 3:
        return -1
    adc = spi.xfer2([1, (8 + channel) << 4, 0])  # Send read command
    data = ((adc[1] & 3) << 8) | adc[2]  # Combine bytes for 10-bit value
    return data

# Main loop to read values
try:
    print("Reading MCP3008... Press Ctrl+C to stop.")
    while True:
        mic_value = read_adc(0)  # Read from CH0
        print(f"CH0 Raw Value: {mic_value}")
        time.sleep(0.5)

except KeyboardInterrupt:
    print("\nStopping...")

finally:
    spi.close()  # Ensure SPI is closed properly
