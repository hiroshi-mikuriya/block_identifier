import sys
import cv2
import numpy as np

# for picamera
import picamera
from fractions import Fraction
import io
import time

class BlockInfo:
  def __init__(self):
    self.bgr = [0, 0, 0] 
    self.hsv = [0, 0, 0]
    self.rc = [0, 0, 0, 0]
    self.color_area = [0, 0, 0, 0]
    self.color = ""
    self.width = 0
  def __repr__(self):
    return "<bgr %s : hsv %s : rc %s : color_area %s : color %s : width %s>\n" % (self.bgr, self.hsv, self.rc, self.color_area, self.color, self.width)

class Option:
  def __init__(self):
    self.stub_th = 20
    self.size_th = 190
    self.bin_th = 200
    self.camera_width = int(720 * 0.9)
    self.camera_height = int(480 * 0.9)
    self.block_height = 20
    self.block_width = 16
  def __repr__(self):
    return "<stub_th %s : size_th %s : bin_th %s : camera_width %s : camera_height %s : camera_ratio %s : block_height %s : block_width %s>\n" % (self.stub_th, self.size_th, self.bin_th, self.camera_width, self.camera_height, self.camera_ratio, self.block_height, self.block_width)    

class BlockIdentifier:

  @staticmethod
  def __fill_divided_blocks(bin):
    kernel = np.array([[1], [1], [1]], np.uint8)
    ite = 5
    return cv2.erode(cv2.dilate(bin, kernel, ite), kernel, ite)
  
  @staticmethod
  def __get_maximum_contour(contours):
    if len(contours) is 0:
      return []
    contour = contours[0]
    max_area = -1
    for c in contours:
      area = cv2.contourArea(c)
      if max_area < area:
        contour = c
        max_area = area
    return contour
  
  @staticmethod
  def __get_block_contour(img, opt):
    hsv = cv2.split(cv2.cvtColor(img, cv2.COLOR_BGR2HSV))
    _, mixed = cv2.threshold(cv2.min(hsv[1], hsv[2]), 80, 255, cv2.THRESH_BINARY)
    _, block = cv2.threshold(cv2.max(mixed, hsv[2]), opt.bin_th, 255, cv2.THRESH_BINARY)
    # cv2.imshow("block", block)
    filled = BlockIdentifier.__fill_divided_blocks(block)
    contours, _ = cv2.findContours(filled, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # cv2.imshow("original", img)
    # cv2.imshow("mixed", mixed)
    # cv2.imshow("h", hsv[0])
    # cv2.imshow("s", hsv[1])
    # cv2.imshow("v", hsv[2])
    return BlockIdentifier.__get_maximum_contour(contours)
  
  @staticmethod
  def __get_first_border(m, th, rng):
    for y in rng:
      if(th <= m[y][0]):
        return y
    return rng[-1]
  
  @staticmethod
  def __get_top_bottom(bin, th):
    m = cv2.reduce(bin, 1, 1)
    top = BlockIdentifier.__get_first_border(m, th, range(m.shape[0]))
    bottom = BlockIdentifier.__get_first_border(m, th, list(reversed(range(m.shape[0]))))
    return top, bottom

  @staticmethod
  def __get_left_right(bin, th):
    m = cv2.reduce(bin, 0, 1).transpose()
    left = BlockIdentifier.__get_first_border(m, th, range(m.shape[0]))
    right = BlockIdentifier.__get_first_border(m, th, list(reversed(range(m.shape[0]))))
    return left, right
  
  @staticmethod
  def __get_center_rect(rc, ratio):
    x = int(rc[0] + rc[2] * (1 - ratio) / 2)
    y = int(rc[1] + rc[3] * (1 - ratio) / 2)
    w = int(rc[2] * ratio)
    h = int(rc[3] * ratio)
    return [x, y, w, h]
  
  @staticmethod
  def __get_block_color(img, rc):
    a = img[rc[1]:(rc[1] + rc[3]), rc[0]:(rc[0] + rc[2])]
    a = cv2.reduce(a, 0, 1)
    bgr = cv2.reduce(a, 1, 1)
    hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)
    return bgr[0][0], hsv[0][0]
  
  @staticmethod
  def __get_color(hsv):
    return "white" if hsv[1] < 0x40 \
      else "brown" if hsv[0] < 0x07 \
      else "orange" if hsv[0] < 0x11 \
      else "yellow" if hsv[0] < 0x1E \
      else "yellowgreen" if hsv[0] < 0x38 \
      else "green" if hsv[0] < 0x50 \
      else "blue" if hsv[0] < 0x80 \
      else "red"
  
  @staticmethod
  def __get_unit_block(img, bin, y, opt):
    dst = BlockInfo()
    trim = bin[y:y + opt.block_height, 0:bin.shape[1]]
    left, right = BlockIdentifier.__get_left_right(trim, opt.size_th)
    dst.rc = [left, y, right - left, opt.block_height]
    dst.color_area = BlockIdentifier.__get_center_rect(dst.rc, 0.2)
    dst.width = int((dst.rc[2] + opt.block_width / 2) / opt.block_width)
    dst.bgr, dst.hsv = BlockIdentifier.__get_block_color(img, dst.rc)
    dst.color = BlockIdentifier.__get_color(dst.hsv)
    return dst
  
  @staticmethod
  def __get_block_info(contour, img, opt):
    if len(contour) is 0:
      return []
    bin = np.zeros((img.shape[0], img.shape[1], 1), np.uint8)
    cv2.drawContours(bin, [contour], 0, 255, -1)
    cv2.imshow("bin", bin)
    top, bottom = BlockIdentifier.__get_top_bottom(bin, opt.stub_th)
    blockCount = int((bottom - top + opt.block_height / 2) / opt.block_height)
    if blockCount < 0:
      return
    dst = []
    for i in range(blockCount):
      y = int((top * (blockCount - i) + bottom * i) / blockCount)
      dst.append(BlockIdentifier.__get_unit_block(img, bin, y, opt))
    return dst

  @staticmethod
  def calc(img, opt):
    contour = BlockIdentifier.__get_block_contour(img, opt)
    return BlockIdentifier.__get_block_info(contour, img, opt)
  
  @staticmethod
  def __draw_rect(img, rc, color, thick):
    pt1 = (rc[0], rc[1])
    pt2 = (rc[0] + rc[2], rc[1] + rc[3])
    cv2.rectangle(img, pt1, pt2, color, thick)

  @staticmethod
  def show_blocks(img, blocks):
    canvas = np.zeros((img.shape[0], img.shape[1] + 640, 3), np.uint8)
    canvas[0:img.shape[0], 0:img.shape[1], :] = img
    for info in blocks:
      BlockIdentifier.__draw_rect(canvas, info.rc, (0, 255, 0), 1)
      BlockIdentifier.__draw_rect(canvas, info.color_area, (0, 255, 0), 1)
      pt = (int(img.shape[1] * 1.1), int(info.rc[1] + info.rc[3] * 0.4))
      str = "{:12s} W:{:d} R:{:02X} G:{:02X} B:{:02X} H:{:02X} S:{:02X} V:{:02X}".format(info.color, info.width, info.bgr[2], info.bgr[1], info.bgr[0], info.hsv[0], info.hsv[1], info.hsv[2])
      cv2.putText(canvas, str, pt, cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255))
    cv2.imshow("blocks", canvas)



if __name__ == '__main__':
  opt = Option()
  camera = picamera.PiCamera()
  camera.hflip = True
  camera.vflip = True
  camera.resolution = (opt.camera_width, opt.camera_height)
  camera.start_preview()
  camera.awb_mode = 'off'
  camera.awb_gains = [Fraction(411, 256), Fraction(215, 128)]
  time.sleep(3) # wait for adjusting exposure
  camera.exposure_mode = 'off'
  while(True):
    stream = io.BytesIO()
    camera.capture(stream, format='jpeg')
    data = np.fromstring(stream.getvalue(), dtype=np.uint8)
    img = cv2.imdecode(data, 1)
    img = img[0:img.shape[0], img.shape[1]/3:img.shape[1]*2/3]
    if img is None or img.shape[0] is 0:
      print('failed to open image')
      quit()
    blocks = BlockIdentifier.calc(img, opt)
    # print(blocks)
    BlockIdentifier.show_blocks(img, blocks)
    cv2.waitKey(10)

