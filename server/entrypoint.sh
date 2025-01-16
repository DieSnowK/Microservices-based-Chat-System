#!/bin/bash
# Usage: ./entrypoint.sh -h 127.0.0.1 -p 3306,2379,6379 -c '/SnowK/bin/xx_server -flagfile=./xx.conf'

# Usage: Wait_For ip port
Wait_For()
{
    while ! nc -z $1 $2
    do
        echo "$2 port connection failed, waiting..."
        sleep 1
    done

    echo "$1:$2 detected successfully"
}

# 1.Parameter analysis
declare ip
declare ports
declare command

while getopts "h:p:c:" arg
do
    case $arg in
        h)
            ip=$OPTARG;;
        p)
            ports=$OPTARG;;
        c)
            command=$OPTARG;;
    esac
done

# 2.Port detection && Port splitting
#   ${ports//,/ } -> replace , in the string with spaces
#   Array in shell: string separated by spaces
for port in ${ports//,/ } 
do
    Wait_For $ip $port
done

# 3.Execute command
#   eval performs a secondary check on a string and executes it as a command
eval $command