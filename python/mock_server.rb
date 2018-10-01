# frozen_string_literal: true

require 'sinatra'

set :port, 5001

post '/api/show' do
  p JSON.parse(request.body.read, symbolize_names: true)
  'ok'
end
