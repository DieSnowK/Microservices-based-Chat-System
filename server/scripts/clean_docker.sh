#!/bin/bash

declare CONTAINERS_ID
declare IMAGES_ID

CONTAINERS_ID=$(docker container ps -aq)
IMAGES_ID=$(docker images | grep 'server-.*_server' | awk '{print $3}')

docker container stop $CONTAINERS_ID
docker container rm $CONTAINERS_ID
docker image rm $IMAGES_ID