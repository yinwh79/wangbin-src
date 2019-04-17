#!/bin/sh

echo "remake cgi..."
make clean
make


echo "remove old cgi..."

echo "copy new cgi..."
cp ./httpd.cgi ../../../../rootfs/www/cgi-bin/


