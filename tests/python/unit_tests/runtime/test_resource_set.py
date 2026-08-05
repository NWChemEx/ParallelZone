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


class ResourceSetTestCase(unittest.TestCase):
    def test_default_ctor(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.defaulted.is_mine())
        self.assertFalse(self.defaulted.has_ram())

    def test_value_ctor(self):
        self.assertFalse(self.rs.empty())
        self.assertTrue(self.rs.is_mine())
        self.assertTrue(self.rs.has_ram())
        self.assertGreaterEqual(self.rs.mpi_rank(), 0)

    def test_mpi_rank(self):
        self.assertGreaterEqual(self.rs.mpi_rank(), 0)

    def test_is_mine(self):
        self.assertFalse(self.defaulted.is_mine())
        self.assertTrue(self.rs.is_mine())

    def test_has_ram(self):
        self.assertFalse(self.defaulted.has_ram())
        self.assertTrue(self.rs.has_ram())

    def test_ram(self):
        """
        Here we test three things:
        1. Default initialized instances throw if you try to get the RAM.
        2. The value returned by ``self.rs`` is non-null.
        3. We can not use ``ram()`` to bypass read-only nature of the return
        """

        self.assertRaises(Exception, self.defaulted.ram)

        my_ram = self.rs.ram()
        default_ram = pz.hardware.RAM()
        self.assertNotEqual(my_ram, default_ram)

        my_ram = default_ram
        self.assertEqual(my_ram, default_ram)
        self.assertNotEqual(self.rs.ram(), default_ram)

    def test_logger(self):
        """
        Default-initialized instances have no logger object, so we check that
        ``self.defaulted`` throws. For ``self.rs`` we check that the resulting
        Logger has the correct state and that we can write to it (although
        since we can't set the sink in Python we can't verify that it is
        indeed the same sink as the one in the logger in ``self.rs``).
        """

        self.assertRaises(Exception, self.defaulted.logger)
        self.assertEqual(self.rs.logger(), pz.Logger())
        self.rs.logger().info("Hello")

    def test_null(self):
        self.assertTrue(self.defaulted.null())
        self.assertFalse(self.rs.null())

    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.rs.empty())

    def test_comparisons(self):
        other_default = pz.runtime.ResourceSet()

        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        self.assertNotEqual(self.defaulted, self.rs)
        self.assertFalse(self.defaulted == self.rs)

    def setUp(self):
        self.defaulted = pz.runtime.ResourceSet()
        self.rv = pz.runtime.RuntimeView()
        self.rs = self.rv.my_resource_set()
