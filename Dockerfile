# syntax=docker/dockerfile:1

ARG BASE_IMAGE=ubuntu:24.04

###############################################################################
# toolchain
###############################################################################
FROM ${BASE_IMAGE} AS toolchain

RUN --mount=target=/var/lib/apt/lists,type=cache,sharing=locked \
    --mount=target=/var/cache/apt,type=cache,sharing=locked \
    rm -f /etc/apt/apt.conf.d/docker-clean \
    && echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache \
    && apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y --no-install-recommends \
        bzip2 \
        ca-certificates \
        file \
        g++ \
        gcc \
        git \
        libmpc-dev \
        libmpfr-dev \
        make \
        texinfo \
        wget \
        zlib1g-dev \
    && apt-get autoremove -yq

ARG N64_INST=/n64_toolchain
ARG BUILD_PATH=/tmp/build
ARG DOWNLOAD_PATH=/tmp/download
ENV N64_INST=${N64_INST}
ENV BUILD_PATH=${BUILD_PATH}
ENV DOWNLOAD_PATH=${DOWNLOAD_PATH}

COPY ./libdragon/tools/build-toolchain.sh ./libdragon/tools/build-gdb.sh /tools/

RUN --mount=target=${DOWNLOAD_PATH},type=cache,sharing=locked \
    /tools/build-toolchain.sh && \
    /tools/build-gdb.sh && \
    rm -rf ${N64_INST}/share/locale/*

COPY ./libdragon /libdragon
RUN cd /libdragon && ./build.sh

###############################################################################
# devcontainer
###############################################################################
FROM ${BASE_IMAGE} AS devcontainer

RUN --mount=target=/var/lib/apt/lists,type=cache,sharing=locked \
    --mount=target=/var/cache/apt,type=cache,sharing=locked \
    rm -f /etc/apt/apt.conf.d/docker-clean \
    && echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        git \
        make \
    && apt-get autoremove -yq

ARG N64_INST=/n64_toolchain
ENV N64_INST=${N64_INST}

COPY --from=toolchain ${N64_INST} ${N64_INST}

WORKDIR /workspace

CMD ["bash"]
