FROM ubuntu:noble

RUN apt-get update && apt-get upgrade -y \
    && apt-get install -y build-essential cmake clang-format

ENTRYPOINT /bin/bash