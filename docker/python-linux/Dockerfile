# syntax=docker/dockerfile:1.3

ARG PYTHON_TAG
FROM python:${PYTHON_TAG} as build-base
RUN set -eux; \
    wget -O /usr/share/keyrings/adoptium.asc https://packages.adoptium.net/artifactory/api/gpg/key/public; \
    echo "deb [signed-by=/usr/share/keyrings/adoptium.asc] https://packages.adoptium.net/artifactory/deb $(awk -F= '/^VERSION_CODENAME/{print$2}' /etc/os-release) main" | tee /etc/apt/sources.list.d/adoptium.list; \
    apt-get update; \
    apt-get install -y temurin-8-jdk maven; \
    python -m venv /jpy-build-venv; \
    /jpy-build-venv/bin/pip install wheel

ARG TARGETARCH
ENV JAVA_HOME=/usr/lib/jvm/temurin-8-jdk-${TARGETARCH} \
    VIRTUAL_ENV='/jpy-build-venv' \
    PATH="/jpy-build-venv/bin:${PATH}"

FROM build-base as src-base
COPY . /jpy

FROM src-base
RUN set -eux; \
    echo $JAVA_HOME; \
    cd /jpy; \
    python setup.py bdist_wheel
