#!/bin/bash

declare depends
declare SVR_DIR
SVR_DIR=$(pwd)

Get_Depends()
{
    depends=$(ldd $1 | awk '{if (match($3,"/")){print $3}}')

    if [ ! -d "$2" ]; then
        mkdir -p $2
    fi

    cp -Lr $depends $2
}

Make()
{
    if [ ! -d "$1" ]; then
        mkdir -p $1
    fi

    cd $1
    cmake ..
    make -j
    cd $SVR_DIR
}

Make ./core/file/build/
Make ./core/friend/build/
Make ./core/gateway/build/
Make ./core/message/build/
Make ./core/speech/build/
Make ./core/transmite/build/
Make ./core/user/build/

Get_Depends ./core/file/build/file_server ./core/file/depends
Get_Depends ./core/friend/build/friend_server ./core/friend/depends
Get_Depends ./core/gateway/build/gateway_server ./core/gateway/depends
Get_Depends ./core/message/build/message_server ./core/message/depends
Get_Depends ./core/speech/build/speech_server ./core/speech/depends
Get_Depends ./core/transmite/build/transmite_server ./core/transmite/depends
Get_Depends ./core/user/build/user_server ./core/user/depends

cp /bin/nc ./core/file/
Get_Depends /bin/nc ./core/file/depends

cp /bin/nc ./core/friend/
Get_Depends /bin/nc ./core/friend/depends

cp /bin/nc ./core/gateway/
Get_Depends /bin/nc ./core/gateway/depends

cp /bin/nc ./core/message/
Get_Depends /bin/nc ./core/message/depends

cp /bin/nc ./core/speech/
Get_Depends /bin/nc ./core/speech/depends

cp /bin/nc ./core/transmite/
Get_Depends /bin/nc ./core/transmite/depends

cp /bin/nc ./core/user/
Get_Depends /bin/nc ./core/user/depends
