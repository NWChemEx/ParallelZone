# Copyright 2022 NWChemEx-Project
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

# You can build this image by passing BASE_IMAGE, REPO_NAME, BRANCH_NAME, and GITHUB_TOKEN arguments:
# docker build --platform linux/amd64 -t test_build \
#                            --build-arg BASE_IMAGE=nwchemex-project/parallelzone_gcc11 \ 
#                            --build-arg REPO_NAME=ParallelZone \
#                            --build-arg BRANCH_NAME=master \
#                            --build-arg GITHUB_TOKEN=XXX .

ARG         BASE_IMAGE
FROM        ${BASE_IMAGE}

ARG         REPO_NAME
ARG         BRANCH_NAME=master
ARG         GITHUB_TOKEN

# ${BASE_IMAGE} should have the repo in /app/${REPO_NAME}
WORKDIR     /app/${REPO_NAME}
RUN         git pull 
RUN         git checkout ${BRANCH_NAME}
RUN         git pull 
# Build...
RUN         cmake --build build
WORKDIR     /app/${REPO_NAME}/build
# Tests run faster with one thread.
ENV         MAD_NUM_THREADS=1
# OpenMPI requires these env vars
ENV         OMPI_ALLOW_RUN_AS_ROOT=1
ENV         OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
# Run the tests. This could be seperated from the build as well.
RUN         ctest  -VV
