import sys
import cv2
import numpy as np

img = cv2.imread("../images/Thu Jun  7 203903 2018.png", 1)
if img is None or img.shape[0] is 0:
  print('failed to open image')
  quit()

def fillDividedBlocks(bin):
  kernel = np.array([[1], [1], [1]], np.uint8)
  iterations = 5
  dil = cv2.dilate(bin, kernel, iterations)
  return cv2.erode(dil, kernel, iterations)

def getMaximumContour(contours):
  contour = contours[0]
  max_area = -1
  for i in range(0, len(contours)):
    area = cv2.contourArea(contours[i])
    if max_area < area:
      contour = contours[i]
  return contour

def getBlockContour(img):
  hsv = cv2.split(cv2.cvtColor(img, cv2.COLOR_BGR2HSV))
  mixed = cv2.addWeighted(hsv[1], 0.6, hsv[2], 1.0, 0)
  _, block = cv2.threshold(mixed, 200, 255, cv2.THRESH_BINARY)
  cv2.imshow("block", block)
  filled = fillDividedBlocks(block)
  _, contours, _ = cv2.findContours(filled, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
  cv2.imshow("original", img)
  cv2.imshow("mixed", mixed)
  cv2.imshow("h", hsv[0])
  cv2.imshow("s", hsv[1])
  cv2.imshow("v", hsv[2])
  return getMaximumContour(contours)

def getFirstBorder(m, r):
  for y in r:
    if(20 <= m[y][0]):
      return y
  return 0

def getTopBottom(bin):
  m = cv2.reduce(bin, 1, cv2.REDUCE_AVG)
  top = getFirstBorder(m, range(m.shape[0]))
  bottom = getFirstBorder(m, reversed(range(m.shape[0])))
  return top, bottom

def getUnitBlock(bin, y):
  return [0, y, 0, 0] # TODO

def getBlockInfo(contour, img):
  bin = np.zeros((img.shape[0], img.shape[1], 1), np.uint8)
  cv2.drawContours(bin, [contour], 0, 255, -1)
  cv2.imshow("bin", bin)
  top, bottom = getTopBottom(bin)
  blockCount = int((bottom - top + 20 / 2) / 20)
  if blockCount < 0:
    return
  dst = []
  for i in range(blockCount):
    y = (top * (blockCount - i) + bottom * i) / blockCount
    dst.append(getUnitBlock(bin, y))
  return dst

contour = getBlockContour(img)
info = getBlockInfo(contour, img)
print(info)

cv2.waitKey(0)
