#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

# Set magic variables for current file & dir
__dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p "${__dir}/dist"

docker buildx build --build-arg PYTHON_VERSION=3.10 --platform linux/arm64 --target artifact --output type=local,dest="${__dir}/dist/" "${__dir}"
docker buildx build --build-arg PYTHON_VERSION=3.9 --platform linux/arm64 --target artifact --output type=local,dest="${__dir}/dist/" "${__dir}"
docker buildx build --build-arg PYTHON_VERSION=3.8 --platform linux/arm64 --target artifact --output type=local,dest="${__dir}/dist/" "${__dir}"
docker buildx build --build-arg PYTHON_VERSION=3.7 --platform linux/arm64 --target artifact --output type=local,dest="${__dir}/dist/" "${__dir}"
docker buildx build --build-arg PYTHON_VERSION=3.6 --platform linux/arm64 --target artifact --output type=local,dest="${__dir}/dist/" "${__dir}"
