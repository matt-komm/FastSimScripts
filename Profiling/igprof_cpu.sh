#!/bin/bash
echo "profiling: "$1
name=$2
if [[ -z "$2" ]]
then
    name=`date +%y-%m-%d_%H-%M-%S`
fi
echo "name: "$name
#(time igprof -d -t cmsRun -pp -z -o igprof.myrun.gz cmsRun $1) >& out.myrun.txt </dev/null
igprof -d -pp -z -o igprof.$name.gz -t cmsRun cmsRun $1
echo "create analysis..."
igprof-analyse -d -v -g igprof.$name.gz >& igprof.$name.res
igprof-analyse --sqlite -d -v -g igprof.$name.gz | sqlite3 igprof.$name.sql3
echo "done!"
