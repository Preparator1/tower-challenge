#!/bin/bash

docker_compose_setup() {
cat << __EOF__ > "docker-compose.yml"
version: '3.8'

services:
  osrm:
    image: osrm/osrm-backend
    container_name: osrm
    ports:
      - "5000:5000"
    volumes:
      - ./data:/data
    command: >
      osrm-routed --algorithm mld /data/czech-republic-latest.osrm

  opentopodata:
    build: ./opentopodata
    container_name: opentopodata
    ports:
      - "5001:5000"
    volumes:
      - ./opentopodata/data:/app/data
__EOF__
}

opentopodata_setup() {
    mkdir opentopodata && cd opentopodata
    git clone https://github.com/ajnisbet/opentopodata.git .
}

SERVER_DIR="$(dirname $(realpath "$0"))/server"

if ! [[ -d "$SERVER_DIR" ]]; then
    mkdir server && cd server
fi

if ! [[ -f "$SERVER_DIR/docker-compose.yml" ]]; then
    opentopodata_setup
fi