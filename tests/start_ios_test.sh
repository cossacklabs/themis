#!/usr/bin/env bash

START_BUILD_NUMBER=`curl -X POST --header "Accept: application/json" https://circleci.com/api/v1/project/cossacklabs/themis-ios-tests/tree/master\?circle-token=$OBJCTHEMIS_TEST_TOKEN | python -m json.tool | sed -n -e '/"build_num":/ s/^.*"build_num": \(.*\),/\1/p' | sed -n -e '1s/^\([0-9]*\).*/\1/p'`
echo $START_BUILD_NUMBER