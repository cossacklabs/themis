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
    dependencies: [],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .binaryTarget(name: "themis",
                      url: "https://github.com/julepka/themis/releases/download/2.2.1/themis.xcframework.zip",
                      // $swift package compute-checksum output/themis.xcframework.zip
                      checksum: "b4a8d8868cbd09499d75a10315d1231281d40dee11d1fe549cf4893967f3ee38"),

    ]
)
