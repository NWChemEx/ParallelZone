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


def get_runtime():
    return pz.runtime.RuntimeView()


class TestQuickstartTestCase(unittest.TestCase):

    def test_quickstart(self):
        # 99.9% of the time you will be using an existing RuntimeView
        rt = get_runtime()

        # Figure out how many resource sets there are total
        n_rs = rt.size()

        # Get the resource set containing resources local to the current process
        my_rs = rt.my_resource_set()

        # Figure out the mpi rank of the current resource set
        my_rs_rank = my_rs.mpi_rank()

        # Figure out how much RAM the local process has access to
        my_ram = my_rs.ram().total_space()

        self.assertEqual(rt, pz.runtime.RuntimeView())
        self.assertGreater(n_rs, 0)
        self.assertGreaterEqual(my_rs_rank, 0)
        self.assertGreater(my_ram, 0)
