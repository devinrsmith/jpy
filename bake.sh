#!/usr/bin/env bash
docker buildx bake --set "*.output=type=local,dest=/tmp/dist"

