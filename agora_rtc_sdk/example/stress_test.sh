#!/bin/bash -xe
#./stress_test.sh 20   2>1.txt
ulimit  -c unlimited
export LD_LIBRARY_PATH=./../agora_sdk

idx=0
while [ 1 ];
do
        ./out/StressTest  --testtime $1 --sleeptime 5000   >/dev/null 2>&1 &
        echo "${idx}"
        let idx+=1
    sleep 1
done

