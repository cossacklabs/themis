require 'net/http'
require 'base64'
require 'rbthemis'

message_maker = Themis::Smessage.new(
  Base64.decode64('UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x'),
  Base64.decode64('VUVDMgAAAC1z7NYRAhVEhIw6HfdrOohZ/3vhub/LRD4Sve+b4f27Dtsb5WJt')
)

message = Base64.encode64(message_maker.wrap('Hello Themis CI server'))

uri = URI('https://themis.cossacklabs.com/api/riYeXaehvsGkcXa/')
req = Net::HTTP::Post.new(uri)
req.set_form_data('message' => message)
req.content_type = 'application/x-www-form-urlencoded'
res = Net::HTTP.start(uri.hostname, uri.port, use_ssl: uri.scheme == 'https') do |http|
  response = http.request(req)
  puts response.code
  puts response.message
  puts message_maker.unwrap(response.body)
end
