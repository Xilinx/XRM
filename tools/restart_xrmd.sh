#!/bin/bash

echo "systemctl stop xrmd"
systemctl stop xrmd

echo "systemctl start xrmd"
systemctl start xrmd
