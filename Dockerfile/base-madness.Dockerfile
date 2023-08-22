# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

# Install mpi
RUN    apt-get update \
        && DEBIAN_FRONTEND=noninteractive apt-get install -y \
                openmpi-bin \
                libopenmpi-dev 
ARG MADNESS_VERSION

# Install MADNESS
RUN cd /tmp \
    && git clone https://github.com/m-a-d-n-e-s-s/madness.git \
    && cd madness \
    && git checkout ${MADNESS_VERSION} \
    && export BUILD_TARGET=MADworld \
    && export FIND_TARGET=MADworld \
    && cmake -DENABLE_UNITTESTS=OFF -DMADNESS_BUILD_MADWORLD_ONLY=ON -DMADNESS_ENABLE_CEREAL=ON -DENABLE_MKL=OFF -DENABLE_ACML=OFF -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=/install -Bbuild . \
    && cmake --build build \
    && cmake --build build --target install \
    && rm -rf /tmp/madness
