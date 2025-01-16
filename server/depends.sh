#!/bin/bash

declare depends
Get_Depends()
{
    depends=$(ldd $1 | awk '{if (match($3,"/")){print $3}}')

    if [ ! -d "$2" ]; then
        mkdir -p $2
    fi

    cp -Lr $depends $2
}

Get_Depends ./file/build/file_server ./file/depends
Get_Depends ./friend/build/friend_server ./friend/depends
Get_Depends ./gateway/build/gateway_server ./gateway/depends
Get_Depends ./message/build/message_server ./message/depends
Get_Depends ./speech/build/speech_server ./speech/depends
Get_Depends ./transmite/build/transmite_server ./transmite/depends
Get_Depends ./user/build/user_server ./user/depends

cp /bin/nc ./file/
Get_Depends /bin/nc ./file/depends

cp /bin/nc ./friend/
Get_Depends /bin/nc ./friend/depends

cp /bin/nc ./gateway/
Get_Depends /bin/nc ./gateway/depends

cp /bin/nc ./message/
Get_Depends /bin/nc ./message/depends

cp /bin/nc ./speech/
Get_Depends /bin/nc ./speech/depends

cp /bin/nc ./transmite/
Get_Depends /bin/nc ./transmite/depends

cp /bin/nc ./user/
Get_Depends /bin/nc ./user/depends
