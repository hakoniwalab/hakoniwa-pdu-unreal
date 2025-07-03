#!/bin/bash

DOCKER_IMAGE=`cat docker_image.txt`

#TODO REMOVE HERE
#cp /home/${USER}/.ssh/id_rsa .

docker build -t ${DOCKER_IMAGE} -f Dockerfile .

#TODO REMOVE HERE
#rm -f id_rsa
