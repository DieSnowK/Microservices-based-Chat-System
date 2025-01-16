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