#!/bin/sh

if [ ! -f ./xdevctl ]; then
    echo "./xdevctl missing"
    exit 1
fi

if [ ! -c /dev/xdev ]; then
    echo "/dev/xdev missing"
    exit 1
fi

./xdevctl full
echo "write to /dev/xdev in $(./xdevctl inspect)"
echo "it's a test!" > /dev/xdev
echo "read from /dev/xdev in $(./xdevctl inspect)"
cat /dev/xdev | head -c100 | md5sum
echo

./xdevctl null
echo "write to /dev/xdev in $(./xdevctl inspect)"
echo "it's a test!" > /dev/xdev
echo "read from /dev/xdev in $(./xdevctl inspect)"
cat /dev/xdev | head -c100 | md5sum
echo

./xdevctl zero
echo "write to /dev/xdev in $(./xdevctl inspect)"
echo "it's a test!" > /dev/xdev
echo "read from /dev/xdev in $(./xdevctl inspect)"
cat /dev/xdev | head -c100 | md5sum
echo

exit 0
