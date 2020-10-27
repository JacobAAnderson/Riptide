#!/bin/bash

echo "Starting pAcommsHandler"
pAcommsHandler  /opt/riptide/config/whoi-acomms.moos &> /dev/null &

echo "Starting Data Loger"
./Jake/bin/datalogger &> Jake/logs/$(date +%Y-%m-%d_%H%M%S)_DataLogger_output.txt &

echo "Starting Acomms Logger"
./Jake/bin/acoms &> /dev/null &

echo "Done"


# This part must come last!!!
# Exiting the TestController will also terminate this script
echo " "
echo " "
echo " "
echo "__________________________________________"
echo "Start Test Contrller and set altimeter range"
echo "To set altimiter range enter an<range>"
echo "  Example, set range to 100 meters: an100"
echo " "
echo "Use control + C to exit"
echo " "
"TestController"
