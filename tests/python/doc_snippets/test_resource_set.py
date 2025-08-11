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


class TestResourceSetTestCase(unittest.TestCase):
    def test_resourceset(self):
        rv = pz.runtime.RuntimeView()

        # Get the ResourceSet for rank 0
        rank_0_rs = rv.at(0)

        # Get the ResourceSet containing resources local to the current process
        my_rs = rv.my_resource_set()

        # Determine if the rank_0_rs is local or not
        is_local = rank_0_rs.is_mine()

        # Get the local RAM
        my_ram = my_rs.ram()

        # Get a view of the RAM owned by rank 0
        ram_0 = rank_0_rs.ram()

        # Do I locally have RAM?
        do_i_have_ram = my_rs.has_ram()

        # Does rank 0 have RAM?
        does_0_have_ram = rank_0_rs.has_ram()

        # Have each resource set print whether it's rank 0 or not
        my_rs.logger().debug("Am I rank 0: " + str(is_local))

        # TODO: Better tests
        self.assertIsNotNone(rank_0_rs)
        self.assertIsNotNone(my_rs)
        self.assertIsNotNone(my_ram)
        self.assertIsNotNone(ram_0)
        self.assertTrue(do_i_have_ram)
        self.assertTrue(does_0_have_ram)
