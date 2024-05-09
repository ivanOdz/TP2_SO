#!/bin/bash

docker start TPE
docker exec -it TPE make clean -C/root/Toolchain
docker exec -it TPE make clean -C/root/
docker stop TPE
