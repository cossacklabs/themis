#!/bin/bash


while true; do
    BUILD_STATUS=`curl --header "Accept: application/json" https://circleci.com/api/v1/project/mnaza/objcthemis-tests/$1 | python -c 'import sys; import json; print(json.load(sys.stdin)["status"])'`
    if [[ $? -ne 0 ]]
	echo "Can't get build $1 status"
	then exit 1
    fi
    echo $BUILD_STATUS
    if [[ $BUILD_STATUS = "success" ]] 
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
