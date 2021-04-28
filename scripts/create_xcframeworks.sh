#!/usr/bin/env bash
#
# Shebang to explicitly use bash and not break on macOS boxes with zsh as default shell.
#
# This script generates Themis xcframework for iOS and macOS.
# Run it from the repo root so the xcodebuild command finds Themis.xcodeproj

set -eu # common flags to ensure that the shell does not ignore failures

BUILD_PATH=${BUILD_PATH:-build}
output_dir=$BUILD_PATH/xcf_output
project_dir=$(pwd) #repo root where Themis.xcodeproj and Package.swift are

# creating required xcframework structure
mkdir -p $output_dir/archives
mkdir -p $output_dir/iphoneos
mkdir -p $output_dir/macosx

# build the framework for iOS devices
xcodebuild archive \
-scheme "Themis (iOS)" \
-destination="iOS" \
-archivePath $output_dir/archives/ios.xcarchive \
-derivedDataPath $output_dir/iphoneos \
-sdk iphoneos \
SKIP_INSTALL=NO \
BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

# build the framework for iOS simulator
xcodebuild archive \
-scheme "Themis (iOS)" \
-destination="iOS Simulator" \
-archivePath $output_dir/archives/iossimulator.xcarchive \
-derivedDataPath $output_dir/iphoneos \
-sdk iphonesimulator \
SKIP_INSTALL=NO \
BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

# build the framework for macOS
xcodebuild archive \
-scheme "Themis (macOS)" \
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

# zip the xcodeframework
# SPM accepts binary targets only in zip format
cd $output_dir
zip -r themis.xcframework.zip themis.xcframework

rm -rf themis.xcframework

# calculate checksum from the directory with Package.swift
# update the the checksum in Package.swift if that is a new release
cd $project_dir
echo "XCF Checksum:"
swift package compute-checksum $output_dir/themis.xcframework.zip