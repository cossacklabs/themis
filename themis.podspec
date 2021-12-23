Pod::Spec.new do |s|
    s.name = "themis"
    s.version = "0.14.0"
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
        # OpenSSL 1.1.1k
        so.dependency 'CLOpenSSL-XCF', '1.1.11101'

        # Enable bitcode for OpenSSL in a very specific way, but it works, thanks to @deszip
        so.ios.pod_target_xcconfig = {
            'OTHER_CFLAGS[config=Debug]'                => '$(inherited) -fembed-bitcode-marker',
            'OTHER_CFLAGS[config=Release]'              => '$(inherited) -fembed-bitcode',
            'BITCODE_GENERATION_MODE[config=Release]'   => 'bitcode',
            'BITCODE_GENERATION_MODE[config=Debug]'     => 'bitcode-marker'
        }

        # We're building some C code here which uses includes as it pleases.
        # Allow this behavior, but we will have to control header mappings.
        # Also, configure some preprocessor definitions to select OpenSSL backend.
        so.ios.xcconfig = {
            'OTHER_CFLAGS' => '-DLIBRESSL',
            'USE_HEADERMAP' => 'NO',
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/include" "${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
            'CLANG_ALLOW_NON_MODULAR_INCLUDES_IN_FRAMEWORK_MODULES' => 'YES',
        }
        so.osx.xcconfig = {
            'OTHER_CFLAGS' => '-DLIBRESSL',
            'USE_HEADERMAP' => 'NO',
            'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/themis/include" "${PODS_ROOT}/themis/src" "${PODS_ROOT}/themis/src/wrappers/themis/Obj-C"',
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
                "include/themis/*.h",
                "include/soter/*.h",
                "src/themis/*.{c,h}",
                "src/soter/*.{c,h}",
                "src/soter/ed25519/*.{c,h}",
                "src/soter/openssl/*.{c,h}",
            ]
            # Prevent CocoaPods from flattening the headers, we need structure.
            ss.header_mappings_dir = "."
            # Don't export Themis Core headers, make only ObjcThemis public.
            ss.private_header_files = [
                "include/themis/*.h",
                "include/soter/*.h",
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
end
