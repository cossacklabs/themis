Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.10.1"
    s.summary = "Data security library for network communication and data storage for iOS and mac OS"
    s.description = "Themis is a data security library, providing users with high-quality security services for secure messaging of any kinds and flexible data storage. Themis is aimed at modern development practices, with high level OOP wrappers for iOS / macOS, NodeJS, Go, Ruby, Python, PHP and Java / Android. It is designed with ease of use in mind, high security and cross-platform availability."
    s.homepage = "https://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}    

    # will update to particular tag on next release
    #s.source = { :git => "https://github.com/cossacklabs/themis.git", :tag => "#{s.version}" }

    # this commit has fixes in soter that allow to support BoringSSL for iOS
    # more: https://github.com/cossacklabs/themis/pull/330
    s.source = { :git => "https://github.com/cossacklabs/themis.git", :commit => 'efe288ebaaababa1c7adc633e3f8cf325e3db0be'}

    s.author = {'cossacklabs' => 'info@cossacklabs.com'}

    s.module_name = 'themis'
    s.default_subspec = 'themis-openssl'

    s.ios.deployment_target = '8.0'
    s.osx.deployment_target = '10.9'
    s.ios.frameworks = 'UIKit', 'Foundation'


    # use `themis/themis-openssl` as separate target to use Themis with OpenSSL
    s.subspec 'themis-openssl' do |so|
        # Enable bitcode for openssl only, unfortunately boringssl with bitcode not available at the moment
        # 'bitcode-marker' directive omits bitcode payload in binary for debug builds
        so.pod_target_xcconfig = {
            'OTHER_CFLAGS[config=Debug]'                => '$(inherited) -fembed-bitcode-marker',
            'OTHER_CFLAGS[config=Release]'              => '$(inherited) -fembed-bitcode',
            'BITCODE_GENERATION_MODE[config=Release]'   => 'bitcode',
            'BITCODE_GENERATION_MODE[config=Debug]'     => 'bitcode-marker'
        }

        so.dependency 'GRKOpenSSLFramework', '~> 1.0.1'

        so.ios.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
        'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }
        
        so.osx.xcconfig = { 'OTHER_CFLAGS' => '-DLIBRESSL', 'USE_HEADERMAP' => 'NO', 
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }


        # don't use as independent target
        so.subspec 'core' do |ss|
            ss.source_files = "src/themis/*.{h,c}", "src/soter/*.{c,h}", "src/soter/ed25519/*.{c,h}", "src/soter/openssl/*.{c,h}"
            ss.header_mappings_dir = "src"
            ss.header_dir = 'src'
            ss.preserve_paths = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/openssl/*.h"
            ss.public_header_files = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/openssl/*.h"
        end
        
        # don't use as independent target
        so.subspec 'objcwrapper' do |ss|
            ss.header_mappings_dir = 'src/wrappers/themis/Obj-C/objcthemis'
            ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{m,h}"
            ss.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
            ss.header_dir = 'objcthemis'
            ss.dependency 'themis/themis-openssl/core'
        end
    end


    # use `themis/themis-boringssl` as separate target to use Themis with BoringSSL
    s.subspec 'themis-boringssl' do |so|

        so.dependency 'BoringSSL', '~> 10.0'

        so.ios.xcconfig = { 'OTHER_CFLAGS' => '-DBORINGSSL -DCRYPTO_ENGINE_PATH=boringssl -DSOTER_BORINGSSL_DISABLE_XTS', 'USE_HEADERMAP' => 'NO', 
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }
            
        so.osx.xcconfig = { 'OTHER_CFLAGS' => '-DBORINGSSL -DCRYPTO_ENGINE_PATH=boringssl -DSOTER_BORINGSSL_DISABLE_XTS', 'USE_HEADERMAP' => 'NO', 
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"', 'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES' }
        
        # don't use as independent target
        so.subspec 'core' do |ss|
            ss.source_files = "src/themis/*.{h,c}", "src/soter/*.{c,h}", "src/soter/ed25519/*.{c,h}", "src/soter/boringssl/*.{c,h}"
            ss.header_mappings_dir = "src"
            ss.header_dir = 'src'
            ss.preserve_paths = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/boringssl/*.h"
            ss.public_header_files = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/boringssl/*.h"
        end

        # don't use as independent target
        so.subspec 'objcwrapper' do |ss|
            ss.header_mappings_dir = 'src/wrappers/themis/Obj-C/objcthemis'
            ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{m,h}"
            ss.public_header_files = 'src/wrappers/themis/Obj-C/objcthemis/*.h'
            ss.header_dir = 'objcthemis'
            ss.dependency 'themis/themis-boringssl/core'
        end
    end
end
