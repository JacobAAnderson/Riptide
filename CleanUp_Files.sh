#!/bin/bash

echo "Clean Up Data Files"

echo "Removing MOOS Logs"

rm -r MOOS*

echo "Removing pAcommsHandler logs"
rm pAcommsHandler*
rm Jake/pAcommsHandler*

echo "Removing Acomms and Data logs"
rm Jake/logs/*csv
rm Jake/logs/*txt

echo "Done"
