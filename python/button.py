import RPi.GPIO as GPIO
import time

class button:
  def __init__(self, gpio):
    self.gpio = gpio
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(self.gpio, GPIO.IN, GPIO.PUD_DOWN)
  
  def wait_for_push(self):
    def sampling(count):
      dst = 0
      for _ in range(count):
        dst += 0 if GPIO.input(self.gpio) is GPIO.LOW else 1
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

  def close(self):
    GPIO.cleanup()

  def __del__(self):
    self.close()
