Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.2.0"
    s.summary = "themis for iOS and OS X"
    s.description = "Supports OSX and iOS including Simulator (armv7,armv7s,arm64,i386,x86_64)."
    s.homepage = "http://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}
    s.source = { :git => "https://github.com/cossacklabs/themis.git" }
    s.author = {'cossacklabs' => 'info@cossacklabs.com'}
    s.dependency 'OpenSSL-Universal', '~> 1.0.1l'

    s.ios.platform = :ios, '7.0'
    s.ios.deployment_target = '7.0'

    s.osx.platform = :ios, '10.9'
    s.osx.deployment_target = '10.9'
    
    s.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL' } 

    s.subspec 'themis' do |ss|
        ss.source_files = "src/themis/*.{h,c}"
        ss.public_header_files = 'src/themis/*.h'
        ss.header_dir = 'themis'
    end
    
    s.subspec 'soter' do |ss|
        ss.source_files = "src/soter/*.{h,c}"
        ss.public_header_files = 'src/soter/*.h'
        ss.header_dir = 'soter'
        ss.dependency 'themis/themis'
        ss.dependency 'themis/soter_openssl'
    end

    s.subspec 'soter_openssl' do |ss|
        ss.source_files = "src/soter/openssl/*.{h,c}"
        ss.public_header_files = 'src/soter/openssl/*.h'
        ss.header_dir = 'soter/openssl'
        ss.dependency 'themis/themis'
    end

    s.subspec 'objcthemis' do |ss|
        ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{h,m}"
        ss.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
        ss.header_dir = 'objcthemis'        
        ss.dependency 'themis/themis'
        ss.dependency 'themis/soter'
        ss.dependency 'themis/soter_openssl'
    end
end
