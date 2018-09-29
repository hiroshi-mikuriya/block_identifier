import picamera
from fractions import Fraction
import io
import time
import cv2
import numpy as np

class camera:
  def __init__(self, resolution):
    print('initializing camera.')
    self.cam = picamera.PiCamera()
    self.cam.hflip = True
    self.cam.vflip = True
    self.cam.resolution = resolution
    self.cam.awb_mode = 'off'
    self.cam.awb_gains = [Fraction(411, 256), Fraction(215, 128)]
    self.cam.start_preview()
    print('adjusting exposure.')
    time.sleep(2) # wait for adjusting exposure
    self.cam.exposure_mode = 'off'

  def get(self):
    stream = io.BytesIO()
    self.cam.capture(stream, format='bmp')
    data = np.fromstring(stream.getvalue(), dtype=np.uint8)
    return cv2.imdecode(data, 1)

  def close(self):
    self.cam.close()

  def __del__(self):
    self.close()