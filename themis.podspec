Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.9.1"
    s.summary = "Data security library for network communication and data storage for iOS and OS X "
    s.description = "Themis is a data security library, providing users with high-quality security services for secure messaging of any kinds and flexible data storage. Themis is aimed at modern development practices, with high level OOP wrappers for Ruby, Python, PHP, Java / Android and iOS / OSX. It is designed with ease of use in mind, high security and cross-platform availability."
    s.homepage = "http://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}
    s.source = { :git => "https://github.com/cossacklabs/themis.git", :tag => "0.9.1" }
    s.author = {'cossacklabs' => 'info@cossacklabs.com'}
    s.dependency 'OpenSSL-Universal', '~> 1.0.1l'

    s.ios.platform = :ios, '7.0'
    s.ios.deployment_target = '7.0'

    s.osx.platform = :ios, '10.9'
    s.osx.deployment_target = '10.9'
    
    s.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO'} 

    
    s.subspec 'core' do |ss|
        ss.source_files = "src/themis/*.{h,c}", "src/soter/**/*.{c,h}"
        ss.header_mappings_dir = "src"
        ss.public_header_files = "src/themis/*.h", "src/soter/**/*.h"
    end
    
    s.subspec 'objcwrapper' do |ss|
        ss.header_mappings_dir = 'src/wrappers/themis/Obj-C/objcthemis'
        ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{m,h}"
        ss.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
        ss.header_dir = 'objcthemis'
        ss.dependency 'themis/core'
    end
end
