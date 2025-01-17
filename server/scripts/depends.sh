#!/bin/bash

declare depends
declare SVR_DIR
SVR_DIR=$(dirname $(realpath $0))/..

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
    # cd $SVR_DIR
}

Make $SVR_DIR/core/file/build/
Make $SVR_DIR/core/friend/build/
Make $SVR_DIR/core/gateway/build/
Make $SVR_DIR/core/message/build/
Make $SVR_DIR/core/speech/build/
Make $SVR_DIR/core/transmite/build/
Make $SVR_DIR/core/user/build/

Get_Depends $SVR_DIR/core/file/build/file_server $SVR_DIR/core/file/depends
Get_Depends $SVR_DIR/core/friend/build/friend_server $SVR_DIR/core/friend/depends
Get_Depends $SVR_DIR/core/gateway/build/gateway_server $SVR_DIR/core/gateway/depends
Get_Depends $SVR_DIR/core/message/build/message_server $SVR_DIR/core/message/depends
Get_Depends $SVR_DIR/core/speech/build/speech_server $SVR_DIR/core/speech/depends
Get_Depends $SVR_DIR/core/transmite/build/transmite_server $SVR_DIR/core/transmite/depends
Get_Depends $SVR_DIR/core/user/build/user_server $SVR_DIR/core/user/depends

cp /bin/nc $SVR_DIR/core/file/
Get_Depends /bin/nc $SVR_DIR/core/file/depends

cp /bin/nc $SVR_DIR/core/friend/
Get_Depends /bin/nc $SVR_DIR/core/friend/depends

cp /bin/nc $SVR_DIR/core/gateway/
Get_Depends /bin/nc $SVR_DIR/core/gateway/depends

cp /bin/nc $SVR_DIR/core/message/
Get_Depends /bin/nc $SVR_DIR/core/message/depends

cp /bin/nc $SVR_DIR/core/speech/
Get_Depends /bin/nc $SVR_DIR/core/speech/depends

cp /bin/nc $SVR_DIR/core/transmite/
Get_Depends /bin/nc $SVR_DIR/core/transmite/depends

cp /bin/nc $SVR_DIR/core/user/
Get_Depends /bin/nc $SVR_DIR/core/user/depends
