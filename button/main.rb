require 'socket'
require './bcm2835'

GPIO = 24

if BCM.bcm2835_init.zero?
  warn 'failed to init bcm2835'
  exit
end

BCM.bcm2835_gpio_fsel(GPIO, 0)
BCM.bcm2835_gpio_set_pud(GPIO, 1)

def sampling(count)
  (count + 1).times.inject do |a, _|
    a + BCM.bcm2835_gpio_lev(GPIO)
  end
end

loop do
  count = 20
  loop { break if sampling(count).zero? }
  loop { break if count == sampling(count) }
  puts 'pushed button!'
  `aplay se_maoudamashii_onepoint23.wav`
  begin
    sock = TCPSocket.open('127.0.0.1', 4123)
    sock.write('push button')
  rescue StandardError => e
    warn e
  end
end
