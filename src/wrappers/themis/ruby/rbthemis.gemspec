Gem::Specification.new do |s|
  s.name        = 'rbthemis'
  s.version     = '0.15.0'
  s.date        = '2023-06-21'
  s.summary     = 'Data security library for network communication and data storage for Ruby'
  s.description = 'Themis is a convenient cryptographic library for data protection. It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including Ruby, JavaScript, iOS/macOS, Python, and Java/Android.'
  s.authors     = ['CossackLabs']
  s.email       = 'dev@cossacklabs.com'
  s.files       = ['lib/rbthemis.rb']
  s.homepage    = 'http://cossacklabs.com/'
  s.license     = 'Apache-2.0'
  s.add_runtime_dependency 'ffi', '~> 1.9', '>= 1.9.8'
  s.requirements << 'libthemis, v0.15.0'
end
