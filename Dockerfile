# syntax=docker/dockerfile:1.4

ARG PYTHON_VERSION
FROM python:${PYTHON_VERSION} as build

RUN set -eux; \
    curl -O https://cdn.azul.com/zulu/bin/zulu-repo_1.0.0-3_all.deb; \
    apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 0xB1998361219BD9C9; \
    apt-get install ./zulu-repo_1.0.0-3_all.deb; \
    apt-get -qq update; \
    apt-get -qq -y --no-install-recommends install patchelf maven zulu8-jdk-headless; \
    rm -rf /var/lib/apt/lists/*

ENV JAVA_HOME=/usr/lib/jvm/zulu8-ca-arm64
ENV JPY_LOG_LEVEL=DEBUG
ENV CI=true

COPY . .

RUN set -eux; \
    .github/env/Linux/bdist-wheel.sh

FROM scratch as artifact
COPY --from=build dist/ out
