#!/bin/sh

which mono > /dev/null 2>&1
if type mono > /dev/null 2>&1; then
echo "mono is found."

else
echo "mono is not found"

#osascript -e 'tell app "System Events" to display dialog "mono is not found. Some functions can not be used."'


fi

cd `dirname $0`
cd ../Resources
./Effekseer
