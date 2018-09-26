DATA.each do |d|
  d.chomp!
  puts %(print('#{d}', camera.#{d}))
end

__END__
analog_gain
annotate_background
annotate_foreground
annotate_frame_num
annotate_text
annotate_text_size
awb_gains
awb_mode
brightness
clock_mode
closed
color_effects
contrast
crop
digital_gain
drc_strength
exif_tags
exposure_compensation
exposure_mode
exposure_speed
flash_mode
frame
framerate
framerate_delta
framerate_range
hflip
image_denoise
image_effect
image_effect_params
iso
led
meter_mode
overlays
preview
preview_alpha
preview_fullscreen
preview_layer
preview_window
previewing
raw_format
recording
resolution
revision
rotation
saturation
sensor_mode
sharpness
shutter_speed
still_stats
timestamp
vflip
video_denoise
video_stabilization
zoom