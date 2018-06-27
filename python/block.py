import sys
import cv2
import numpy as np

"""
認識したブロックの情報を持つ型
"""
class BlockInfo:
  def __init__(self):
    self.rgb = [0, 0, 0] # ブロックのRGB値
    self.hsv = [0, 0, 0] # ブロックのHSV値
    self.rc = [0, 0, 0, 0] # ブロックの矩形
    self.color_area = [0, 0, 0, 0] # ブロック色判定領域
    self.color = "" # 色名
    self.width = 0 # ブロック幅
  def __repr__(self):
    return "<rgb %s : hsv %s : rc %s : color_area %s : color %s : width %s>\n" % (self.rgb, self.hsv, self.rc, self.color_area, self.color, self.width)

"""
ブロック認識をするための設定パラメータ
"""
class Option:
  def __init__(self):
    self.block_height = 20
    self.block_width = 16
    self.stub_th = 20 # 最上段ブロックのぼっちを除去する閾値
    self.size_th = 190 # ブロック幅判定閾値
    self.bin_th = 200 # ２値化閾値
    self.ratio_s = 0.6 # HSVのS要素の重み（ブロック認識で使用）
    self.ratio_v = 1.0 # HSVのV要素の重み（ブロック認識で使用）
  def __repr__(self):
    return "<block_height %s : block_width %s : stub_th %s : size_th %s : bin_th %s : ratio_s %s : ratio_v %s>\n" % (self.block_height, self.block_width, self.stub_th, self.size_th, self.bin_th, self.ratio_s, self.ratio_v)    

"""
ブロック認識を行うクラス
"""
class BlockIdentifier:

  @staticmethod
  def __fill_divided_blocks(bin):
    kernel = np.array([[1], [1], [1]], np.uint8)
    ite = 5
    return cv2.erode(cv2.dilate(bin, kernel, ite), kernel, ite)
  
  @staticmethod
  def __get_maximum_contour(contours):
    contour = contours[0]
    max_area = -1
    for i in range(0, len(contours)):
      area = cv2.contourArea(contours[i])
      if max_area < area:
        contour = contours[i]
        max_area = area
    return contour
  
  @staticmethod
  def __get_block_contour(img, opt):
    hsv = cv2.split(cv2.cvtColor(img, cv2.COLOR_BGR2HSV))
    mixed = cv2.addWeighted(hsv[1], opt.ratio_s, hsv[2], opt.ratio_v, 0)
    _, block = cv2.threshold(mixed, opt.bin_th, 255, cv2.THRESH_BINARY)
    # cv2.imshow("block", block)
    filled = BlockIdentifier.__fill_divided_blocks(block)
    _, contours, _ = cv2.findContours(filled, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
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
    m = cv2.reduce(bin, 1, cv2.REDUCE_AVG)
    top = BlockIdentifier.__get_first_border(m, th, range(m.shape[0]))
    bottom = BlockIdentifier.__get_first_border(m, th, list(reversed(range(m.shape[0]))))
    return top, bottom

  @staticmethod
  def __get_left_right(bin, th):
    m = cv2.reduce(bin, 0, cv2.REDUCE_AVG).transpose()
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
    a = cv2.reduce(img, 0, cv2.REDUCE_AVG)
    rgb = cv2.reduce(a, 1, cv2.REDUCE_AVG)
    hsv = cv2.cvtColor(rgb, cv2.COLOR_BGR2HSV)
    return rgb[0][0], hsv[0][0]
  
  @staticmethod
  def __get_unit_block(img, bin, y, opt):
    dst = BlockInfo()
    trim = bin[y:y + opt.block_height, 0:bin.shape[1]]
    left, right = BlockIdentifier.__get_left_right(trim, opt.size_th)
    dst.rc = [left, y, right - left, opt.block_height]
    dst.color_area = BlockIdentifier.__get_center_rect(dst.rc, 0.2)
    dst.width = int((dst.rc[2] + opt.block_width / 2) / opt.block_width)
    rgb, hsv = BlockIdentifier.__get_block_color(img, dst.rc)
    dst.rgb = rgb
    dst.hsv = hsv
    return dst
  
  @staticmethod
  def __get_block_info(contour, img, opt):
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


if __name__ == '__main__':
  img = cv2.imread("../images/red2.png", 1)
  if img is None or img.shape[0] is 0:
    print('failed to open image')
    quit()
  blocks = BlockIdentifier.calc(img, Option())
  print(blocks)

