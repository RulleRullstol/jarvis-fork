import spidev

spi = spidev.SpiDev()
spi.open(10, 0)  # Ensure correct SPI Bus (use 10,0 based on previous tests)
spi.max_speed_hz = 500000  # Reduce speed for stability

# Send dummy bytes to check if SPI responds
response = spi.xfer2([0xAA, 0x00, 0x00])  # 0xAA is test data
print(f"SPI Response: {response}")

spi.close()
