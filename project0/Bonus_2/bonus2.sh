#!/bin/sh
touch bonus2output
awk '{printf $1 " "}' bonus2input.txt >>bonus2output
echo "" >>bonus2output
awk '{printf $2 " "}' bonus2input.txt >>bonus2output
echo "" >>bonus2output
cat bonus2output
