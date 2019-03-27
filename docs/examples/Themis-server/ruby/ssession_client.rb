require 'net/http'
require 'base64'
require 'rbthemis'

class CallbacksForThemis < Themis::Callbacks
  def get_pub_key_by_id(id)
    {
      'mjXppqhCzZglREZ' => Base64.decode64('VUVDMgAAAC1z7NYRAhVEhIw6HfdrOohZ/3vhub/LRD4Sve+b4f27Dtsb5WJt')
    }[id]
  end
end

tr = CallbacksForThemis.new
session = Themis::Ssession.new(
  'riYeXaehvsGkcXa',
  Base64.decode64('UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x'),
  tr
)

message = Base64.encode64(session.connect_request)

uri = URI('https://themis.cossacklabs.com/api/riYeXaehvsGkcXa/')
res = Net::HTTP.start(uri.hostname, uri.port, use_ssl: uri.scheme == 'https') do |http|
  until session.established?
    req = Net::HTTP::Post.new(uri)
    req.set_form_data('message' => message)
    req.content_type = 'application/x-www-form-urlencoded'
    response = http.request(req)
    message = Base64.encode64(session.unwrap(response.body)[1])
  end
  message = Base64.encode64(session.wrap('Hello Themis CI server'))
  req = Net::HTTP::Post.new(uri)
  req.set_form_data('message' => message)
  req.content_type = 'application/x-www-form-urlencoded'
  response = http.request(req)
  puts session.unwrap(response.body)[1]
end
