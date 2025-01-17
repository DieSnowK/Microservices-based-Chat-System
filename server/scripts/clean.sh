#!/bin/bash

declare SVR_DIR
SVR_DIR=$(dirname $(realpath $0))/..

Clean_Svr()
{
    cd $1
    rm nc build/ depends/ -rf
    # cd $SVR_DIR
}

Clean_Svr $SVR_DIR/core/file
Clean_Svr $SVR_DIR/core/friend
Clean_Svr $SVR_DIR/core/gateway
Clean_Svr $SVR_DIR/core/message
Clean_Svr $SVR_DIR/core/speech
Clean_Svr $SVR_DIR/core/transmite
Clean_Svr $SVR_DIR/core/user