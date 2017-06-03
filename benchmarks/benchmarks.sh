#!/bin/bash
TIMEFORMAT='%3R'

function benchmark {
    echo $1
    echo "" > $1.csv
    for ((i=200; i<=$2; i+=100)); do 
	echo -ne $i \\r
	A=`{ time ./$1-openssl $i; } 2>&1 1>/dev/null`
	B=`{ time ./$1-libsodium $i; } 2>&1 1>/dev/null`
	for j in {2..10}
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
