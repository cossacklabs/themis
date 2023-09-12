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
                      url: "https://github.com/cossacklabs/themis/releases/download/0.15.1/themis.xcframework.zip",
                      // The scripts/create_xcframework.sh calculates the checksum when generating the XCF.
                      // Alternatively, run from package directory:
                      // swift package compute-checksum build/xcf_output/themis.xcframework.zip
                      checksum: "2d5bfbba7d5d3ecd0d40acdaee65d1b5d42dee5df5ae9d99faec4119087238c5"),

    ]
)
