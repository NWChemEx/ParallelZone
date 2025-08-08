#
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
#

import unittest

import parallelzone as pz


class LoggerTestCase(unittest.TestCase):
    """
    Testing the logger from Python is difficult because we have not
    exposed any constructors beyond the default ctor (in practice PZ's
    runtime component is responsible for constructing the logger objects,
    and that happens in C++). Point is, without access to the constructors
    we can't really grab the sink in the logger.
    """

    def setUp(self):
        severity = pz.Logger.severity

        self.defaulted = pz.Logger()
        self.log = pz.runtime.RuntimeView().logger()
        self.severities = [
            severity.trace,
            severity.debug,
            severity.info,
            severity.warn,
            severity.error,
            severity.critical,
        ]

    def test_set_severity(self):
        self.defaulted.set_severity(pz.Logger.severity.debug)
        self.log.set_severity(pz.Logger.severity.debug)

    def test_trace(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.trace("Hello").trace("World!")
        self.log.trace("Hello").trace("World!")

    def test_debug(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.debug("Hello").debug("World!")
        self.log.debug("Hello").debug("World!")

    def test_info(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.info("Hello").info("World!")
        self.log.info("Hello").info("World!")

    def test_warn(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.warn("Hello").warn("World!")
        self.log.warn("Hello").warn("World!")

    def test_error(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.error("Hello").error("World!")
        self.log.error("Hello").error("World!")

    def test_critical(self):
        """
        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.critical("Hello").critical("World!")
        self.log.critical("Hello").critical("World!")

    def test_log1(self):
        """
        This method tests the overload of the Logger class which only
        takes a string, no severity level.

        N.B. we chain the calls to make sure the returns work.
        """

        self.defaulted.log("Hello").log("World!")
        self.log.log("Hello").log("World!")

    def test_log2(self):
        """
        This method tests the overload of the Logger class which takes the
        severity level and a string.

        N.B. we chain the calls to make sure the returns work.
        """

        for level in self.severities:
            self.defaulted.log(level, "Hello").log(level, "World!")
            self.log.log(level, "Hello").log(level, "World!")

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pz.Logger())
        self.assertFalse(self.defaulted != pz.Logger())

        self.assertNotEqual(self.defaulted, self.log)
        self.assertFalse(self.defaulted == self.log)
