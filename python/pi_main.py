import json
import cv2
from camera import camera
from block import block
from button import button

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, block.info):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

btn = button(24)
opt = block.option(0.9)
cam = camera((opt.camera_width, opt.camera_height))

try:
  while True:
    btn.wait_for_push()
    img = cam.get()
    if img is None or img.shape[0] is 0:
      print('failed to open image')
      quit()
    img = img[0:img.shape[0], img.shape[1]/3:img.shape[1]*2/3]
    try:
      blocks = block.calc(img, opt)
      block.show_blocks(img, blocks)
      print(json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder))
    except cv2.error as e:
      print(e)
except KeyboardInterrupt:
  cam.close()
  btn.close()