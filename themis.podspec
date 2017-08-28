Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.9.5"
    s.summary = "Data security library for network communication and data storage for iOS and mac OS"
    s.description = "Themis is a data security library, providing users with high-quality security services for secure messaging of any kinds and flexible data storage. Themis is aimed at modern development practices, with high level OOP wrappers for iOS / macOS, NodeJS, Go, Ruby, Python, PHP and Java / Android. It is designed with ease of use in mind, high security and cross-platform availability."
    s.homepage = "http://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}    
    s.source = { :git => "https://github.com/cossacklabs/themis.git", :tag => "#{s.version}" }
    s.author = {'cossacklabs' => 'info@cossacklabs.com'}
    
    #s.dependency 'OpenSSL-Universal', '1.0.2.10'

    s.ios.vendored_frameworks  = 'src/wrappers/themis/Obj-C/openssl_frameworks/ios/openssl.framework'
    s.osx.vendored_frameworks  = 'src/wrappers/themis/Obj-C/openssl_frameworks/macos/openssl.framework'


    s.ios.deployment_target = '8.0'
    s.osx.deployment_target = '10.9'
    s.requires_arc = false
    
    s.ios.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
        'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
        'CLANG_ENABLE_MODULES' => 'NO', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }
        
    s.osx.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
        'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
        'CLANG_ENABLE_MODULES' => 'NO', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }    
        

    #s.ios.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
    #    'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
    #    'LIBRARY_SEARCH_PATHS' => '"${PODS_ROOT}/OpenSSL-Universal/lib-ios"' }
    #    
    #s.osx.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
    #    'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
    #    'LIBRARY_SEARCH_PATHS' => '"${PODS_ROOT}/OpenSSL-Universal/lib-macos"' } 


    # open ssl
    # s.libraries = 'ssl', 'crypto'
    
    s.subspec 'core' do |ss|
        ss.source_files = "src/themis/*.{h,c}", "src/soter/*.{c,h}", "src/soter/ed25519/*.{c,h}", "src/soter/openssl/*.{c,h}"
        ss.header_mappings_dir = "src"
        ss.header_dir = 'src'
        ss.preserve_paths = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/openssl/*.h"
        ss.public_header_files = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/openssl/*.h"
    end
    
    s.subspec 'objcwrapper' do |ss|
        ss.header_mappings_dir = 'src/wrappers/themis/Obj-C/objcthemis'
        ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{m,h}"
        ss.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
        ss.header_dir = 'objcthemis'
        ss.dependency 'themis/core'
    end
end
