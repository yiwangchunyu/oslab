#!/bin/sh

#builds a new directory  and  $1 is an argument(can be 'foo') which can input from the terminal 
mkdir $1 

#echo to a file by redirection and name.txt is created automatically
echo 'Wang Chunyu' >name.txt 

#echo to a file by redirection and syno.txt is created automatically
echo '10152150127' >stno.txt  

#makes a copy in the directory of foo
cp name.txt foo 

#makes a copy in the directory of foo
cp stno.txt foo  



