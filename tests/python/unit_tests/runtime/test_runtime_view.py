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

class RuntimeViewTestCase(unittest.TestCase):
    def test_default_ctor(self):
        self.assertGreater(self.defaulted.size(), 0)
        self.assertFalse(self.defaulted.did_i_start_mpi())

    def test_size(self):
        self.assertGreater(self.defaulted.size(), 0)

    def test_did_i_start_mpi(self):
        self.assertFalse(self.defaulted.did_i_start_mpi())

    def test_at(self):
        """
        Here we check that:
        1. A bounds error is raised for unacceptable indices.
        2. The resulting object is not empty (without exposing MPI to Python we
           can't manually create a ResourceSet in the correct state to compare
           to).
        3. We verify that the returned object can not be used to circumvent the
           read-only nature of the return.
        """

        self.assertRaises(IndexError, self.defaulted.at, self.defaulted.size())
        rs0 = self.defaulted.at(0)
        default_rs = pz.runtime.ResourceSet()
        self.assertNotEqual(rs0, default_rs)

        rs0 = default_rs
        self.assertEqual(rs0, default_rs)
        self.assertNotEqual(self.defaulted.at(0), default_rs)


    def test_has_me(self):
        self.assertTrue(self.defaulted.has_me())

    def test_my_resource_set(self):
        """
        Similar to ``test_at``, we check that:
        1. The resulting object is not empty.
        2. The resulting object can not be used to bypass intended read-only
           nature of the return.
        """

        my_rs = self.defaulted.my_resource_set()
        default_rs = pz.runtime.ResourceSet()
        self.assertNotEqual(my_rs, default_rs)

        my_rs = default_rs
        self.assertEqual(my_rs, default_rs)
        self.assertNotEqual(self.defaulted.my_resource_set(), default_rs)


    def test_count_ram(self):
        default_ram = pz.hardware.RAM()
        ram         = self.defaulted.my_resource_set().ram()

        self.assertEqual(self.defaulted.count(default_ram), 0)
        self.assertEqual(self.defaulted.count(ram), 1)

    def test_logger(self):
        self.assertIsNotNone(self.defaulted.logger())
        self.defaulted.logger().log("Hello").log("world")

    def test_stack_callback_1(self):
        is_running = [True]
        
        def turn_off(val=is_running):
            val[0] = False
        
        falls_off = pz.runtime.RuntimeView()
        falls_off.stack_callback(turn_off)
        del falls_off

        self.assertFalse(is_running[0])

    def test_stack_callback_2(self):
        func_no = [1]

        def call_back_1(val=func_no):
            val[0] = val[0] + 1

        def call_back_2(val=func_no):
            val[0] = val[0] * 2

        rt = pz.runtime.RuntimeView()
        rt.stack_callback(call_back_1)
        rt.stack_callback(call_back_2)
        del rt

        self.assertEqual(func_no[0], 3)

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pz.runtime.RuntimeView())
        self.assertFalse(self.defaulted != pz.runtime.RuntimeView())

    def setUp(self):
        self.defaulted = pz.runtime.RuntimeView()
