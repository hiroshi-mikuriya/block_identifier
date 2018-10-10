# frozen_string_literal: true

require 'sinatra'

set :port, 5001

post '/api/show' do
  p req = JSON.parse(request.body.read, symbolize_names: true)
  puts req[:orders]
  'ok'
end
