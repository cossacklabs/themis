#!/bin/bash -e

export TOP_PID=$$
trap "exit 1" TERM
export status=0

function fail {
    printf "\033[1m\033[31m ${2} fail \x1b[0m\n"
}
export -f fail
function success {
    printf "\033[1m\033[32m ${2} success \x1b[0m\n"
}
export -f success

function check_result_zero (){
    result=$?
    if [ ${result} != "0" ]
    then
        fail
        status=1
    else
        success
    fi
}
export -f check_result_zero