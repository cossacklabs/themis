Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.9"
    s.summary = "Data security library for network communication and data storage for iOS and OS X "
    s.description = "Themis is a data security library, providing users with high-quality security services for secure messaging of any kinds and flexible data storage. Themis is aimed at modern development practices, with high level OOP wrappers for Ruby, Python, PHP, Java / Android and iOS / OSX. It is designed with ease of use in mind, high security and cross-platform availability."
    s.homepage = "http://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}
    s.source = { :git => "https://github.com/cossacklabs/themis.git", :tag => "0.9" }
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
