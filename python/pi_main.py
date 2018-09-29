import picamera
from fractions import Fraction
import io
import time
import json
import cv2
from block import *

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, BlockInfo):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

opt = Option()
camera = picamera.PiCamera()
camera.hflip = True
camera.vflip = True
camera.resolution = (opt.camera_width, opt.camera_height)
camera.awb_mode = 'off'
camera.awb_gains = [Fraction(411, 256), Fraction(215, 128)]
camera.start_preview()
time.sleep(3) # wait for adjusting exposure
camera.exposure_mode = 'off'
while(True):
  stream = io.BytesIO()
  camera.capture(stream, format='bmp')
  data = np.fromstring(stream.getvalue(), dtype=np.uint8)
  img = cv2.imdecode(data, 1)
  img = img[0:img.shape[0], img.shape[1]/3:img.shape[1]*2/3]
  if img is None or img.shape[0] is 0:
    print('failed to open image')
    quit()
  try:
    blocks = BlockIdentifier.calc(img, opt)
    BlockIdentifier.show_blocks(img, blocks)
    print(json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder))
    cv2.waitKey(10)
  except cv2.error as e:
    print(e)