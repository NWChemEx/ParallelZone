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

ARG CEREAL_VERSION

# Install cereal
RUN cd /tmp \
    && git clone https://github.com/USCiLab/cereal.git \
    && cd cereal \
    && git checkout tags/v${CEREAL_VERSION} \
    && export BUILD_TARGET=cereal \
    && export FIND_TARGET=cereal \
    && cmake -DJUST_INSTALL_CEREAL=ON -DCMAKE_INSTALL_PREFIX=/install -Bbuild . \
    && cmake --build build \
    && cmake --build build --target install \
    && rm -rf /tmp/cereal
