#!/usr/bin/env bash
#
# Generate Themis XCFramework for iOS and macOS.
#
#     scripts/create_xcframework.sh [--with-clopenss] [--with-boringssl]
#
# Output will be placed into $BUILD_PATH/xcf_output

set -eu

BUILD_PATH=${BUILD_PATH:-build}

clopenssl_scheme_ios="Themis (iOS)"
clopenssl_scheme_mac="Themis (macOS)"
clopenssl_output_dir=$BUILD_PATH/xcf_output/CLOpenSSL

boringssl_scheme_ios="Themis (iOS) - BoringSSL"
boringssl_scheme_mac="Themis (macOS) - BoringSSL"
boringssl_output_dir=$BUILD_PATH/xcf_output/BoringSSL

if [[ (! -d Themis.xcodeproj) || (! -f Package.swift) ]]
then
    echo >&2 "Please launch scripts/create_xcframeworks.sh from Themis repository root"
    exit 1
fi

help() {
    eval "echo \"$(cat $0 | awk 'NR == 3, /^$/ { print substr($0, 3) }')\""
}

build_clopenssl=
build_boringssl=

while [[ $# -gt 0 ]]
do
    case "$1" in
      -h|--help) help; exit;;
      --with-clopenssl) build_clopenssl=yes; shift;;
      --with-boringssl) build_boringssl=yes; shift;;
      *)
        echo >&2 "Unknown option: $1"
        echo >&2
        help
        exit 1
        ;;
    esac
done

if [[ -z "$build_clopenssl" && -z "$build_boringssl" ]]
then
    build_clopenssl=yes
fi

# CLOpenSSL builds expect Carthage dependencies to be fetched.
# If they don't seem to be here, do a favor and pull them now.
if [[ -n "$build_clopenssl" && ! -d Carthage ]]
then
    carthage bootstrap
fi

build_xcf() {
    local output_dir="$1"
    local scheme_ios="$2"
    local scheme_mac="$3"

    # creating required xcframework structure
    mkdir -p $output_dir/archives
    mkdir -p $output_dir/iphoneos
    mkdir -p $output_dir/macosx

    # build the framework for iOS devices
    xcodebuild archive \
        -scheme "$scheme_ios" \
        -destination="iOS" \
        -archivePath $output_dir/archives/ios.xcarchive \
        -derivedDataPath $output_dir/iphoneos \
        -sdk iphoneos \
        SKIP_INSTALL=NO \
        BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

    # build the framework for iOS simulator
    xcodebuild archive \
        -scheme "$scheme_ios" \
        -destination="iOS Simulator" \
        -archivePath $output_dir/archives/iossimulator.xcarchive \
        -derivedDataPath $output_dir/iphoneos \
        -sdk iphonesimulator \
        SKIP_INSTALL=NO \
        BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

    # build the framework for macOS
    xcodebuild archive \
        -scheme "$scheme_mac" \
        -destination="macOS" \
        -archivePath $output_dir/archives/macosx.xcarchive \
        -derivedDataPath $output_dir/macosx \
        -sdk macosx \
        SKIP_INSTALL=NO \
        BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

    # gather separate frameworks into a single xcframework
    xcodebuild -create-xcframework \
        -framework $output_dir/archives/ios.xcarchive/Products/Library/Frameworks/themis.framework \
        -framework $output_dir/archives/iossimulator.xcarchive/Products/Library/Frameworks/themis.framework \
        -framework $output_dir/archives/macosx.xcarchive/Products/Library/Frameworks/themis.framework \
        -output $output_dir/themis.xcframework

    # deleting the artifacts
    rm -rf $output_dir/archives
    rm -rf $output_dir/iphoneos
    rm -rf $output_dir/macosx
}

pack_xcf() {
    local output_dir="$1"
    # zip the xcodeframework
    # SPM accepts binary targets only in zip format
    cd $output_dir
    zip -r themis.xcframework.zip themis.xcframework
    rm -rf themis.xcframework
    cd ~-
}

checksum_xcf() {
    local output_dir="$1"
    # calculate checksum from the directory with Package.swift
    # update the the checksum in Package.swift if that is a new release
    echo "XCF Checksum:"
    swift package compute-checksum $output_dir/themis.xcframework.zip
}

[[ -n "$build_clopenssl" ]] && build_xcf    "$clopenssl_output_dir" "$clopenssl_scheme_ios" "$clopenssl_scheme_mac"
[[ -n "$build_boringssl" ]] && build_xcf    "$boringssl_output_dir" "$boringssl_scheme_ios" "$boringssl_scheme_mac"
[[ -n "$build_clopenssl" ]] && pack_xcf     "$clopenssl_output_dir"
[[ -n "$build_boringssl" ]] && pack_xcf     "$boringssl_output_dir"
[[ -n "$build_clopenssl" ]] && checksum_xcf "$clopenssl_output_dir"
[[ -n "$build_boringssl" ]] && checksum_xcf "$boringssl_output_dir"
