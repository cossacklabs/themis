Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.2.0"
    s.summary = "themis for iOS and OS X"
    s.description = "Supports OSX and iOS including Simulator (armv7,armv7s,arm64,i386,x86_64)."
    s.homepage = "http://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}
    s.source = { :git => "http://ph.cossacklabs.com/diffusion/MAIN/main-repo.git" }
    s.author = {'cossacklabs' => 'info@cossacklabs.com'}
    s.dependency 'OpenSSL-Universal', '~> 1.0.1l'

    s.ios.platform = :ios, '7.0'
    s.ios.deployment_target = '7.0'
    s.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL' } 

    s.subspec 'soter' do |ss|
        ss.ios.source_files = "src/soter/*.{h,c}"
        ss.ios.public_header_files = 'src/soter/*.h'
        ss.ios.header_dir = 'soter'
    end

    s.subspec 'soter_openssl' do |ss|
        ss.ios.source_files = "src/soter/openssl/*.{h,c}"
        ss.ios.public_header_files = 'src/soter/openssl/*.h'
        ss.ios.header_dir = 'soter/openssl'
    end

    s.subspec 'themis' do |ss|
        ss.ios.source_files = "src/themis/*.{h,c}"
        ss.ios.public_header_files = 'src/themis/*.h'
        ss.ios.header_dir = 'themis'
    end

    s.subspec 'objthemis' do |ss|
        ss.ios.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{h,m}"
        ss.ios.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
        ss.ios.header_dir = 'objcthemis'
    end
end