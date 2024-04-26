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


class TestRAMTestCase(unittest.TestCase):

    def test_ram(self):
        rv = pz.runtime.RuntimeView()

        # Get the RAM for rank 0
        rank_0_ram = rv.at(0).ram()

        # Get the RAM local to the current process
        my_ram = rv.my_resource_set().ram()

        # How much total ram does rank_0_ram and my_ram have?
        rank_0_total = rank_0_ram.total_space()
        my_total_ram = my_ram.total_space()

        self.assertIsNotNone(rank_0_ram)
        self.assertIsNotNone(my_ram)

        self.assertGreater(rank_0_total, 0)
        self.assertGreater(my_total_ram, 0)
