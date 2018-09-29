import picamera
from fractions import Fraction
import io
import time
import json
import cv2
import numpy as np
import RPi.GPIO as GPIO
from block import block

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, block.info):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

BUTTON_GPIO = 24
GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_GPIO, GPIO.IN, GPIO.PUD_DOWN)

def wait_for_button_push():
  def sampling(count):
    dst = 0
    for _ in range(count):
      dst += 0 if GPIO.input(BUTTON_GPIO) is GPIO.LOW else 1
      time.sleep(0.01)
    return dst
  COUNT = 5
  print('waiting for button release')
  while True:
    if sampling(COUNT) is 0: break
  print('waiting for button push')
  while True:
    if sampling(COUNT) is COUNT: break
  print('button pushed')
  return

opt = block.option(0.9)
camera = picamera.PiCamera()
camera.hflip = True
camera.vflip = True
camera.resolution = (opt.camera_width, opt.camera_height)
camera.awb_mode = 'off'
camera.awb_gains = [Fraction(411, 256), Fraction(215, 128)]
camera.start_preview()
time.sleep(3) # wait for adjusting exposure
camera.exposure_mode = 'off'

try:
  while True:
    wait_for_button_push()
    stream = io.BytesIO()
    camera.capture(stream, format='bmp')
    data = np.fromstring(stream.getvalue(), dtype=np.uint8)
    img = cv2.imdecode(data, 1)
    img = img[0:img.shape[0], img.shape[1]/3:img.shape[1]*2/3]
    if img is None or img.shape[0] is 0:
      print('failed to open image')
      quit()
    try:
      blocks = block.calc(img, opt)
      block.show_blocks(img, blocks)
      print(json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder))
    except cv2.error as e:
      print(e)
except KeyboardInterrupt:
  camera.close()
  GPIO.cleanup()