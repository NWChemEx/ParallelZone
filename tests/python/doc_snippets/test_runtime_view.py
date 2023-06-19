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
import parallelzone as pz
import unittest

class TestRuntimeViewTestCase(unittest.TestCase):
    def test_runtimeview(self):
        rv = pz.runtime.RuntimeView()
        result = [0, 1, 2]

        # Get the resource set containing resources local to the current process
        my_rs = rv.my_resource_set()

        # Log the value of result as severity::trace
        for x in result:
            rv.logger().trace(str(x))

        # Log the value of result as severity::debug
        debug_s = pz.Logger.severity.debug
        for x in result:
            rv.logger().log(debug_s, str(x))

        self.assertIsNotNone(my_rs)
