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

class RAMTestCase(unittest.TestCase):
    def setUp(self):
        """
        For unit testing purposes we "create" two RAM instances:

        1. defaulted is a default constructed RAM instance.
        2. has_value is the process local RAM instance
        """

        self.defaulted = pz.hardware.RAM()
        self.rv        = pz.runtime.RuntimeView()
        self.rs        = self.rv.my_resource_set()
        self.has_value = self.rs.ram()

    def test_default_ctor(self):
        self.assertTrue(self.defaulted.empty())
        self.assertEqual(self.defaulted.total_space(), 0)

    def test_value_ctor(self):
        self.assertFalse(self.has_value.empty())
        self.assertGreater(self.has_value.total_space(), 0)

    def test_total_space(self):
        self.assertEqual(self.defaulted.total_space(), 0)
        self.assertGreater(self.has_value.total_space(), 0)

    def test_empty(self):
        self.assertTrue(self.defaulted.empty())
        self.assertFalse(self.has_value.empty())

    def test_comparisons(self):
        other_default = pz.hardware.RAM()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        self.assertEqual(self.has_value, self.rv.my_resource_set().ram())
        self.assertFalse(self.has_value != self.rv.my_resource_set().ram())

        self.assertFalse(self.defaulted == self.has_value)
        self.assertTrue(self.defaulted != self.has_value)
