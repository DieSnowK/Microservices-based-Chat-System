#!/bin/bash

declare SVR_DIR
SVR_DIR=$(pwd)

Clean_Svr()
{
    cd $1
    rm nc build/ depends/ -rf
    cd $SVR_DIR
}

Clean_Svr ./core/file
Clean_Svr ./core/friend
Clean_Svr ./core/gateway
Clean_Svr ./core/message
Clean_Svr ./core/speech
Clean_Svr ./core/transmite
Clean_Svr ./core/user