#!/usr/bin/env bash


while true; do
    BUILD_STATUS=`curl --header "Accept: application/json" https://circleci.com/api/v1/project/cossacklabs/themis-ios-tests/$1?circle-token=$OBJCTHEMIS_TEST_TOKEN | python -c 'import sys; import json; print(json.load(sys.stdin)["status"])'`
    if [ "$?" != "0" ]
	then echo "Can't get build $1 status"
	exit 1
    fi
    echo $BUILD_STATUS
    if [[ $BUILD_STATUS = "success" ]] 
	then exit 0
    fi
    if [[ $BUILD_STATUS = "fixed" ]] 
	then exit 0
    fi
    if [[ $BUILD_STATUS = "canceled" ]] 
	then exit 0
    fi

    if [[ $BUILD_STATUS = "failed" ]]
	then exit 1
    fi
    sleep 10
done
