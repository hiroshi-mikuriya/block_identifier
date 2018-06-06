require 'sinatra'
require 'json'

set :bind, '0.0.0.0'

post '/api/show' do
  p params
  'ok'
end
