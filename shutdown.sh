#!/bin/bash

echo "Killing Acomms and Datal loging processes"

pkill "pAcommsHandler /opt/riptide/config/whoi-acomms.moos"
pkill "./Jake/bin/datalogger"
pkill "./Jake/bin/acoms"

echo "Done"
