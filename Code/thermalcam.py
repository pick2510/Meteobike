#!usr/bin/python
import busio
import board
import adafruit_amg88xx

import matplotlib.pyplot as plt

i2c = busio.I2C(board.SCL, board.SDA)
amg = adafruit_amg88xx.AMG88XX(i2c)

plt.ion()
pixels = amg.pixels
graph = plt.imshow(pixels, origin="lower", cmap=plt.cm.jet, interpolation='lanczos', vmin=0, vmax=40)
cbar = plt.colorbar()

try:
    while 1:
        graph.set_data(amg.pixels)
        plt.draw()
        plt.pause(0.1)

except KeyboardInterrupt:
    print("CTRL-C Detected")

finally:
    print("Exit")
