#!/bin/sh

find /bin -name "b*" -ls |awk '{print substr($11,6) " " $5 " " $3}' >>output

#sorted by the file name field (in order of alphabet, from small to large)
sort -k 1 output

#read only for other users
chmod 744 output
