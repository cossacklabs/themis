// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "themis",
    products: [
        .library(
            name: "themis",
            targets: ["themis"]),
    ],
    // OpenSSL XCF is statically linked to Themis XCF, so no need to have it as a dependency
    dependencies: [],
    targets: [
        .binaryTarget(name: "themis",
                      // update version in URL path
                      url: "https://github.com/cossacklabs/themis/releases/download/0.13.8/themis.xcframework.zip",
                      // The scripts/create_xcframework.sh calculates the checksum when generating the XCF.
                      // Alternatively, run from package directory:
                      // swift package compute-checksum build/xcf_output/themis.xcframework.zip
                      checksum: "afda7b2ab0ca86a73f55d309289283e648cd958b5cd9f16dc89d7157317b2165"),

    ]
)
