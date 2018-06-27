import sys
import cv2
import numpy as np

BLOCK_HEIGHT = 20
BLOCK_WIDTH = 16
STUB_TH = 20 # 最上段ブロックのぼっちを除去する閾値
SIZE_TH = 190 # ブロック幅判定閾値
BIN_TH = 200 # ２値化閾値

img = cv2.imread("../images/white1.png", 1)
if img is None or img.shape[0] is 0:
  print('failed to open image')
  quit()

class BlockInfo:
  def __init__(self):
    self.color = [0, 0, 0] # ブロックの色
    self.rc = [0, 0, 0, 0] # ブロックの矩形
    self.color_area = [0, 0, 0, 0] # ブロック色判定領域
    self.ave = [0, 0, 0] # 平均色
    self.width = 0 # ブロック幅

def fill_divided_blocks(bin):
  kernel = np.array([[1], [1], [1]], np.uint8)
  ite = 5
  return cv2.erode(cv2.dilate(bin, kernel, ite), kernel, ite)

def get_maximum_contour(contours):
  contour = contours[0]
  max_area = -1
  for i in range(0, len(contours)):
    area = cv2.contourArea(contours[i])
    if max_area < area:
      contour = contours[i]
      max_area = area
  return contour

def get_block_contour(img):
  hsv = cv2.split(cv2.cvtColor(img, cv2.COLOR_BGR2HSV))
  mixed = cv2.addWeighted(hsv[1], 0.6, hsv[2], 1.0, 0)
  _, block = cv2.threshold(mixed, BIN_TH, 255, cv2.THRESH_BINARY)
  cv2.imshow("block", block)
  filled = fill_divided_blocks(block)
  _, contours, _ = cv2.findContours(filled, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
  cv2.imshow("original", img)
  cv2.imshow("mixed", mixed)
  cv2.imshow("h", hsv[0])
  cv2.imshow("s", hsv[1])
  cv2.imshow("v", hsv[2])
  return get_maximum_contour(contours)

def get_first_border(m, r, th):
  for y in r:
    if(th <= m[y][0]):
      return y
  return r[-1]

def get_top_bottom(bin):
  m = cv2.reduce(bin, 1, cv2.REDUCE_AVG)
  top = get_first_border(m, range(m.shape[0]), STUB_TH)
  bottom = get_first_border(m, list(reversed(range(m.shape[0]))), STUB_TH)
  return top, bottom

def get_unit_block(bin, y):
  dst = BlockInfo()
  trim = bin[y:y + BLOCK_HEIGHT, 0:bin.shape[1]]
  avg = cv2.reduce(trim, 0, cv2.REDUCE_AVG).transpose()
  left = get_first_border(avg, range(avg.shape[0]), SIZE_TH)
  right = get_first_border(avg, list(reversed(range(avg.shape[0]))), SIZE_TH)
  dst.rc = [left, y, right - left, BLOCK_HEIGHT]
  dst.width = int((dst.rc[2] + BLOCK_WIDTH / 2) / BLOCK_WIDTH)
  return dst

def get_block_info(contour, img):
  bin = np.zeros((img.shape[0], img.shape[1], 1), np.uint8)
  cv2.drawContours(bin, [contour], 0, 255, -1)
  cv2.imshow("bin", bin)
  top, bottom = get_top_bottom(bin)
  blockCount = int((bottom - top + BLOCK_HEIGHT / 2) / BLOCK_HEIGHT)
  if blockCount < 0:
    return
  dst = []
  for i in range(blockCount):
    y = int((top * (blockCount - i) + bottom * i) / blockCount)
    dst.append(get_unit_block(bin, y))
  for i in range(len(dst)):
    rc = dst[i].rc
    cv2.rectangle(bin, (rc[0], rc[1]), (rc[0] + rc[2], rc[1] + rc[3]), 128)
  cv2.imshow("blocks", bin)
  return dst

contour = get_block_contour(img)
info = get_block_info(contour, img)

cv2.waitKey(0)
