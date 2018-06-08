require 'sinatra'
require 'json'

set :bind, '0.0.0.0'
set :port, 5001

post '/api/show' do
  p params
  'ok'
end
