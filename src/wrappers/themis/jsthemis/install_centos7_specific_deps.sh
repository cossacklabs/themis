#!/usr/bin/env bash
#
# centos:7 has pretty old Node (v8) which is not supported by many packages,
# including mocha - test framework that we use. Therefore, we need to downgrade
# it for testing. The addon itself works fine, we just cannot test it without
# this patch.


set -e -o pipefail

OS=$(cat /etc/*-release | grep '^NAME' | tr -d 'NAME="' || true)
VERSION=$(cat /etc/*-release | grep '^VERSION_ID' | tr -d 'VERSION_ID="' || true)

if [[ "$OS" == "CentOS Linux" && "$VERSION" == "7" ]]; then
	npm install mocha@7 --save-dev
fi
