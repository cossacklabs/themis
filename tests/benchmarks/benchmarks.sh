#!/bin/bash
#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

TIMEFORMAT='%3R'

function benchmark {
    echo $1
    echo "" > $1.csv
    for ((i=200; i<=$2; i+=100)); do 
	echo -ne $i \\r
	A=`{ time ./$1-openssl $i; } 2>&1 1>/dev/null`
	B=`{ time ./$1-libsodium $i; } 2>&1 1>/dev/null`
	for j in {2..20}
	do
	    AA=`{ time ./$1-openssl $i; } 2>&1 1>/dev/null`
	    A=`echo \($A + $AA\)/2 | bc -l`
	    BB=`{ time ./$1-libsodium $i; } 2>&1 1>/dev/null `
	    B=`echo \($B + $BB\)/2 | bc -l`
	done
	echo $i $A $B | awk '{OFS=";"};{print $1,$2,$3}' >>$1.csv
    done
}


benchmark $1 $2
