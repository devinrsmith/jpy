# syntax=docker/dockerfile:1.4

ARG PYTHON_TAG
FROM python:${PYTHON_TAG} as build-base
RUN set -eux; \
    wget -O /usr/share/keyrings/adoptium.asc https://packages.adoptium.net/artifactory/api/gpg/key/public; \
    echo "deb [signed-by=/usr/share/keyrings/adoptium.asc] https://packages.adoptium.net/artifactory/deb $(awk -F= '/^VERSION_CODENAME/{print$2}' /etc/os-release) main" | tee /etc/apt/sources.list.d/adoptium.list; \
    apt-get update; \
    apt-get install -y temurin-8-jdk maven patchelf; \
    python -m venv /jpy-build-venv; \
    /jpy-build-venv/bin/pip install --upgrade pip; \
    /jpy-build-venv/bin/pip install wheel

ARG TARGETARCH
ENV JAVA_HOME=/usr/lib/jvm/temurin-8-jdk-${TARGETARCH} \
    VIRTUAL_ENV='/jpy-build-venv' \
    PATH="/jpy-build-venv/bin:${PATH}"

FROM build-base as src-base
COPY --link . /jpy

FROM src-base as build
RUN set -eux; \
    cd /jpy; \
    .github/env/Linux/bdist-wheel.sh

FROM scratch
COPY --link --from=build /jpy/dist /dist
