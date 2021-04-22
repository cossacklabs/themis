#!/usr/bin/env bash
#
# Shebang to explicitly use bash and not break on macOS boxes with zsh as default shell.
#
# After that you usually describe what this script is for and how to use it. In particular,
# what it expects the current directory to be. (That's more important if it's it "scripts".)

set -eu # common flags to ensure that the shell does not ignore failures

BUILD_PATH=build
output_dir=$BUILD_PATH/xcf_output

cd ..
repo_root_path=$(pwd)

mkdir $BUILD_PATH
mkdir $output_dir
mkdir $output_dir/archives
mkdir $output_dir/iphoneos
mkdir $output_dir/macosx

xcodebuild archive \
-scheme "Themis (iOS)" \
-destination="iOS" \
-archivePath $output_dir/archives/ios.xcarchive \
-derivedDataPath $output_dir/iphoneos \
-sdk iphoneos \
SKIP_INSTALL=NO \
BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

xcodebuild archive \
-scheme "Themis (iOS)" \
-destination="iOS Simulator" \
-archivePath $output_dir/archives/iossimulator.xcarchive \
-derivedDataPath $output_dir/iphoneos \
-sdk iphonesimulator \
SKIP_INSTALL=NO \
BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

xcodebuild archive \
-scheme "Themis (macOS)" \
-destination="macOS" \
-archivePath $output_dir/archives/macosx.xcarchive \
-derivedDataPath $output_dir/macosx \
-sdk macosx \
SKIP_INSTALL=NO \
BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

xcodebuild -create-xcframework \
-framework $output_dir/archives/ios.xcarchive/Products/Library/Frameworks/themis.framework \
-framework $output_dir/archives/iossimulator.xcarchive/Products/Library/Frameworks/themis.framework \
-framework $output_dir/archives/macosx.xcarchive/Products/Library/Frameworks/themis.framework \
-output $output_dir/themis.xcframework

rm -rf $output_dir/archives
rm -rf $output_dir/iphoneos
rm -rf $output_dir/macosx

cd $output_dir
zip -r themis.xcframework.zip themis.xcframework

rm -rf themis.xcframework

cd "$repo_root_path"
echo "XCF Checksum:"
swift package compute-checksum $output_dir/themis.xcframework.zip