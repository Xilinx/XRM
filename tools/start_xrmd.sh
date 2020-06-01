#!/bin/bash

echo "rm -f /dev/shm/xrm.data"
rm -f /dev/shm/xrm.data

echo "systemctl start xrmd"
systemctl start xrmd
