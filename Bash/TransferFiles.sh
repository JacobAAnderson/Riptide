#!/usr/local/bin/bash

name="Dory"

ip=192.168.1.210

echo " "
echo " "
echo " "
echo "Starting File transfer from Riptide: $name"
echo "At: $ip"
echo " "

echo "---> Getting pAcommsHandler logs"
scp uuuv@$ip:pAcommsHandler* pAcommsHandler/
scp uuuv@$ip:Jake/pAcommsHandler* pAcommsHandler/

rm pAcommsHandler/pAcommsHandler_uuuv_latest.txt
chmod 444 pAcommsHandler/*pAcommsHandler*

echo " "
echo "--> Getting Acomms Logs"
scp uuuv@$ip:Jake/logs/*acommsLog.csv Acoms/
chmod 444 Acoms/*acommsLog.csv

echo " "
echo "--> Getting Data Logs"
scp uuuv@$ip:Jake/logs/*DataLog.csv Data/
scp uuuv@$ip:Jake/logs/*DataLogger_output.txt Data/

chmod 444 Data/*DataLog.csv
chmod 444 Data/*DataLogger_output.txt
echo " "
echo " "
echo "Done"
echo " "
echo " "
echo " "
