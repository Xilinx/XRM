#!/bin/bash

echo "systemctl stop xrmd"
systemctl stop xrmd

echo "rm -f /dev/shm/xrm.data"
rm -f /dev/shm/xrm.data
