import parallelzone as pz
import unittest

class RAMTestCase(unittest.TestCase):
    def setUp(self):
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
