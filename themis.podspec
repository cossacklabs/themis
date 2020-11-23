Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.13.6"
    s.summary = "Data security library for network communication and data storage for iOS and mac OS"
    s.description = "Themis is a convenient cryptographic library for data protection. It provides secure messaging with forward secrecy and secure data storage. Themis is aimed at modern development practices and has a unified API across 12 platforms, including iOS/macOS, Ruby, JavaScript, Python, and Java/Android."
    s.homepage = "https://cossacklabs.com"
    s.license = { :type => 'Apache 2.0'}

    s.source = { :git => "https://github.com/cossacklabs/themis.git", :tag => "#{s.version}" }

    s.author = {'cossacklabs' => 'info@cossacklabs.com'}

    s.module_name = 'themis'
    s.default_subspec = 'openssl-1.1.1'

    s.ios.deployment_target = '10.0'
    s.osx.deployment_target = '10.11'
    s.ios.frameworks = 'UIKit', 'Foundation'

    # TODO(ilammy, 2020-03-02): resolve "pod spec lint" warnings due to dependencies
    # If you update dependencies, please check whether we can remove "--allow-warnings"
    # from podspec validation in .github/workflows/test-objc.yaml

    # This variant uses the current stable, non-legacy version of OpenSSL.
    s.subspec 'openssl-1.1.1' do |so|
        # OpenSSL 1.1.1h
        so.dependency 'CLOpenSSL', '1.1.10802'

        # Enable bitcode for OpenSSL in a very specific way, but it works, thanks to @deszip
        so.ios.pod_target_xcconfig = {
            'OTHER_CFLAGS[config=Debug]'                => '$(inherited) -fembed-bitcode-marker',
            'OTHER_CFLAGS[config=Release]'              => '$(inherited) -fembed-bitcode',
            'BITCODE_GENERATION_MODE[config=Release]'   => 'bitcode',
            'BITCODE_GENERATION_MODE[config=Debug]'     => 'bitcode-marker'
        }

        # As of version 1.1.10801, the framework produced by CLOpenSSL does not
        # contain arm64 slice for iOS Simulator since it conflicts with arm64
        # slice for the real iOS. Fixing this requires migration to XCFrameworks.
        # See T1406 for current status of XCFrameworks.
        so.ios.pod_target_xcconfig  = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
        so.ios.user_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }

        # We're building some C code here which uses includes as it pleases.
        # Allow this behavior, but we will have to control header mappings.
        so.ios.xcconfig = {
            'OTHER_CFLAGS' => '-DLIBRESSL',
            'USE_HEADERMAP' => 'NO',
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
            'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES',
        }
        so.osx.xcconfig = {
            'OTHER_CFLAGS' => '-DLIBRESSL',
            'USE_HEADERMAP' => 'NO',
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
            'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES',
        }

        # We need to do this weird subspec matryoshka because CocoaPods
        # insists on compiling everything as if it were a modular framework.
        # Unfortunately, Themis has a lot of #include "themis/something.h"
        # which break in modular compilation. The "header_dir" fixes it up,
        # but we must set it differently for ObjCThemis. Hence two subspecs.
        # End users should use "themis/openssl-1.1.1" only, not these ones.
        so.subspec 'core' do |ss|
            ss.source_files = [
                "src/themis/*.{c,h}",
                "src/soter/*.{c,h}",
                "src/soter/ed25519/*.{c,h}",
                "src/soter/openssl/*.{c,h}",
            ]
            ss.header_dir = "src"
            ss.header_mappings_dir = "src"
            # Don't export Themis Core headers, make only ObjcThemis public.
            ss.private_header_files = [
                "src/themis/*.h",
                "src/soter/*.h",
                "src/soter/ed25519/*.h",
                "src/soter/openssl/*.h",
            ]
        end
        so.subspec 'objcwrapper' do |ss|
            ss.dependency 'themis/openssl-1.1.1/core'
            ss.source_files = "src/wrappers/themis/Obj-C/objcthemis/*.{m,h}"
            ss.header_dir = "objcthemis"
            ss.header_mappings_dir = "src/wrappers/themis/Obj-C/objcthemis"
            ss.public_header_files = "src/wrappers/themis/Obj-C/objcthemis/*.h"
        end
    end

    # use `themis/themis-openssl` as separate target to use Themis with OpenSSL
    s.subspec 'themis-openssl' do |so|
        # Enable bitcode for OpenSSL in a very specific way, but it works, thanks to @deszip
        #so.ios.pod_target_xcconfig = {'ENABLE_BITCODE' => 'YES' }
        so.ios.pod_target_xcconfig = {
            'OTHER_CFLAGS[config=Debug]'                => '$(inherited) -fembed-bitcode-marker',
            'OTHER_CFLAGS[config=Release]'              => '$(inherited) -fembed-bitcode',
            'BITCODE_GENERATION_MODE[config=Release]'   => 'bitcode',
            'BITCODE_GENERATION_MODE[config=Debug]'     => 'bitcode-marker'
        }

        # As of version 1.0.2.20.1, GRKOpenSSLFramework binaries do not contain
        # arm64 slices for iOS Simulator and macOS, and thus do not support
        # Apple Silicon. Disable building Themis for Apple Silicon until
        # GRKOpenSSLFramework gets proper arm64 support.
        so.ios.pod_target_xcconfig  = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
        so.ios.user_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
        so.osx.pod_target_xcconfig  = { 'EXCLUDED_ARCHS' => 'arm64' }
        so.osx.user_target_xcconfig = { 'EXCLUDED_ARCHS' => 'arm64' }

        # TODO: due to error in symbols in GRKOpenSSLFramework 219 release, we've manually switched to 218
        # which doesn't sound like a good decision, so when GRKOpenSSLFramework will be updated –
        # please bring back correct dependency version
        # https://github.com/cossacklabs/themis/issues/538
        # 26 sept 2019
        #so.dependency 'GRKOpenSSLFramework', '~> 1.0.1' # <-- this is good
        # 11 oct 2020 update: trying 1.0.2.20, but it also gives linking errors, so postponed
        # https://github.com/levigroker/GRKOpenSSLFramework/issues/10
        #so.dependency 'GRKOpenSSLFramework', '1.0.2.20'  # 1.0.2u, latest in 1.0.2 branch

        so.dependency 'GRKOpenSSLFramework', '1.0.2.18'  # <-- this is temp


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
            ss.private_header_files = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/openssl/*.h"
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
            ss.private_header_files = "src/themis/*.h", "src/soter/*.h", "src/soter/ed25519/*.h", "src/soter/boringssl/*.h"
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
