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
        self.assertRaises(IndexError, self.defaulted.at, self.defaulted.size())

    def test_has_me(self):
        self.assertTrue(self.defaulted.has_me())

    def test_my_resource_set(self):
        self.assertIsNotNone(self.defaulted.my_resource_set())

    def test_count_ram(self):
        default_ram = pz.hardware.RAM()
        ram         = self.defaulted.my_resource_set().ram()

        self.assertEqual(self.defaulted.count(default_ram), 0)
        self.assertEqual(self.defaulted.count(ram), 1)

    def test_logger(self):
        self.assertIsNotNone(self.defaulted.logger())

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pz.runtime.RuntimeView())
        self.assertFalse(self.defaulted != pz.runtime.RuntimeView())

    def setUp(self):
        self.defaulted = pz.runtime.RuntimeView()
