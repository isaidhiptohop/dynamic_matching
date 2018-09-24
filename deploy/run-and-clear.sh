#!/bin/bash

if [ "$#" -lt 1 ];
then
    echo "no sequence passed."
    exit 1
fi

sequence=$1
output=""

if [ "$#" -gt 1 ];
then
    at_once=$2
else
    at_once=1000
fi

output="output/"$(echo $sequence | cut -f1 -d"." | cut -f2 -d"/")"_"$at_once"/"

echo "processing $sequence..."
echo "output will be written to $output"

./executer -f $sequence --at-once $at_once -mr 10
rm $output"progress"
./gpa_matching $sequence --step=$at_once --multi-runs=10
./compare $output
#rm -r $output"snapshots"
rm -r $output"matchings"*

