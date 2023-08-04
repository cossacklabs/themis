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
                      url: "https://github.com/radetsky/themis/releases/download/0.15.0/themis.xcframework.zip",
                      // The scripts/create_xcframework.sh calculates the checksum when generating the XCF.
                      // Alternatively, run from package directory:
                      // swift package compute-checksum build/xcf_output/themis.xcframework.zip
                      checksum: "51867ceca2ad499380d3998ff18f80fbe8e60fa2e6392ea1cff9b78a637ee98f"),

    ]
)
