import cv2
import pygame.mixer
from camera import camera
from block import block
from button import button
import sender

def exist_object(blocks):
  for b in blocks:
    if b.width is 2:
      return True
  return False    

pygame.mixer.init()
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
      if exist_object(blocks):
        pygame.mixer.music.load('../sound/atari.wav')
        pygame.mixer.music.play(1)
        sender.post(blocks)
      else:
        pygame.mixer.music.load('../sound/hazure.wav')
        pygame.mixer.music.play(1)
    except cv2.error as e:
      print(e)
except KeyboardInterrupt:
  cam.close()
  btn.close()