# Copyright 2025 NWChemEx-Project
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

import parallelzone as pz
import unittest


class TestLoggerTestCase(unittest.TestCase):

    def test_logging(self):
        rv = pz.runtime.RuntimeView()
        my_rs = rv.my_resource_set()

        log = pz.runtime.RuntimeView().logger()
        severity = pz.Logger.severity
        severities = [
            severity.trace, severity.debug, severity.info, severity.warn,
            severity.error, severity.critical
        ]

        log.set_severity(severity.critical)

        for level in severities:
            log.log(level, "Hello")

        log.set_severity(severity.trace)

        for level in severities:
            log.log(level, "Hello")

        self.assertIsNotNone(my_rs)
