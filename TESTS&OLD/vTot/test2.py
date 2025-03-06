import spidev

spi = spidev.SpiDev()
spi.open(10, 0)  # Adjust for /dev/spidev10.0
spi.max_speed_hz = 1350000

def read_adc(channel):
    adc = spi.xfer2([1, (8 + channel) << 4, 0])
    data = ((adc[1] & 3) << 8) | adc[2]
    return data

for i in range(8):
    print(f"Channel {i}: {read_adc(i)}")

spi.close()