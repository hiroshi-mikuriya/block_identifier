import cv2
import numpy as np

class block:
  class info:
    def __init__(self):
      self.bgr = [0, 0, 0] 
      self.hsv = [0, 0, 0]
      self.rc = [0, 0, 0, 0]
      self.color_area = [0, 0, 0, 0]
      self.color = ""
      self.width = 0
    def __repr__(self):
      return "<bgr %s : hsv %s : rc %s : color_area %s : color %s : width %s>\n" % (self.bgr, self.hsv, self.rc, self.color_area, self.color, self.width)
  
  class option:
    def __init__(self, ratio):
      self.stub_th = 20
      self.size_th = 190
      self.bin_th0 = 70
      self.bin_th1 = 200
      self.camera_width = int(720 * ratio)
      self.camera_height = int(480 * ratio)
      self.block_height = int(22.23 * ratio)
      self.block_width = int(17.78 * ratio)
    def __repr__(self):
      return "<stub_th %s : size_th %s : bin_th0 %s : bin_th1 %s : camera_width %s : camera_height %s : camera_ratio %s : block_height %s : block_width %s>\n" % (self.stub_th, self.size_th, self.bin_th0, self.bin_th1, self.camera_width, self.camera_height, self.camera_ratio, self.block_height, self.block_width)    
  
  @staticmethod
  def __fill_divided_blocks(bin):
    kernel = np.array([[1], [1], [1]], np.uint8)
    ite = 5
    return cv2.erode(cv2.dilate(bin, kernel, ite), kernel, ite)
  
  @staticmethod
  def __get_maximum_contour(contours):
    contour = []
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
    _, mix0 = cv2.threshold(cv2.min(hsv[1], hsv[2]), opt.bin_th0, 255, cv2.THRESH_BINARY)
    _, mix1 = cv2.threshold(cv2.max(mix0, hsv[2]), opt.bin_th1, 255, cv2.THRESH_BINARY)
    # cv2.imshow("mix1", mix1)
    filled = block.__fill_divided_blocks(mix1)
    contours, _ = cv2.findContours(filled, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # cv2.imshow("original", img)
    # cv2.imshow("mix0", mix0)
    # cv2.imshow("h", hsv[0])
    # cv2.imshow("s", hsv[1])
    # cv2.imshow("v", hsv[2])
    return block.__get_maximum_contour(contours)
  
  @staticmethod
  def __get_first_border(m, th, rng):
    for y in rng:
      if(th <= m[y][0]):
        return y
    return rng[-1]
  
  @staticmethod
  def __get_top_bottom(bin, th):
    m = cv2.reduce(bin, 1, 1)
    top = block.__get_first_border(m, th, range(m.shape[0]))
    bottom = block.__get_first_border(m, th, list(reversed(range(m.shape[0]))))
    return top, bottom

  @staticmethod
  def __get_left_right(bin, th):
    m = cv2.reduce(bin, 0, 1).transpose()
    left = block.__get_first_border(m, th, range(m.shape[0]))
    right = block.__get_first_border(m, th, list(reversed(range(m.shape[0]))))
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
      else ("brown" if hsv[2] < 0xA0 else "orange") if hsv[0] < 0x11 \
      else "yellow" if hsv[0] < 0x1E \
      else "yellowgreen" if hsv[0] < 0x38 \
      else "green" if hsv[0] < 0x50 \
      else "blue" if hsv[0] < 0x80 \
      else ("brown" if hsv[2] < 0xA0 else "red")
  
  @staticmethod
  def __get_unit_block(img, bin, y, opt):
    dst = block.info()
    trim = bin[y:y + opt.block_height, 0:bin.shape[1]]
    left, right = block.__get_left_right(trim, opt.size_th)
    dst.rc = [left, y, right - left, opt.block_height]
    dst.color_area = block.__get_center_rect(dst.rc, 0.2)
    dst.width = int((dst.rc[2] + opt.block_width / 2) / opt.block_width)
    dst.bgr, dst.hsv = block.__get_block_color(img, dst.rc)
    dst.color = block.__get_color(dst.hsv)
    return dst
  
  @staticmethod
  def __get_block_info(contour, img, opt):
    dst = []
    if len(contour) is 0:
      return dst
    bin = np.zeros((img.shape[0], img.shape[1], 1), np.uint8)
    cv2.drawContours(bin, [contour], 0, 255, -1)
    # cv2.imshow("bin", bin)
    top, bottom = block.__get_top_bottom(bin, opt.stub_th)
    blockCount = int((bottom - top + opt.block_height / 10) / opt.block_height)
    if blockCount < 0:
      return dst
    top = bottom - blockCount * opt.block_height
    for i in range(blockCount):
      y = int((top * (blockCount - i) + bottom * i) / blockCount)
      dst.append(block.__get_unit_block(img, bin, y, opt))
    return dst

  @staticmethod
  def calc(img, opt):
    contour = block.__get_block_contour(img, opt)
    return block.__get_block_info(contour, img, opt)
  
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
      block.__draw_rect(canvas, info.rc, (0, 255, 0), 1)
      block.__draw_rect(canvas, info.color_area, (0, 255, 0), 1)
      color = (255, 255, 255)
      pt0 = (int(img.shape[1] * 1.1), int(info.rc[1] + info.rc[3] * 0.4))
      cv2.putText(canvas, info.color, pt0, cv2.FONT_HERSHEY_SIMPLEX, 0.7, color)
      pt1 = (pt0[0] + 150, pt0[1])
      str = "W:{:d} R:{:02X} G:{:02X} B:{:02X} H:{:02X} S:{:02X} V:{:02X}".format(info.width, info.bgr[2], info.bgr[1], info.bgr[0], info.hsv[0], info.hsv[1], info.hsv[2])
      cv2.putText(canvas, str, pt1, cv2.FONT_HERSHEY_SIMPLEX, 0.7, color)
    cv2.imshow("blocks", canvas)
    cv2.waitKey(1) # to show image

if __name__ == '__main__':
  import sys
  img = cv2.imread(sys.argv[1], 1)
  if img is None or img.shape[0] is 0:
    print('failed to open image')
    quit()
  opt = block.option(0.9)
  blocks = block.calc(img, opt)
  print(blocks)
  block.show_blocks(img, blocks)
  cv2.waitKey(0)