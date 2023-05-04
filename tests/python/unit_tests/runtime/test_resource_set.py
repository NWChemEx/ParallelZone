import parallelzone as pz
import unittest

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
        self.assertRaises(Exception, self.defaulted.ram)
        self.assertEqual(self.rs.ram(), self.rv.my_resource_set().ram())

    def test_logger(self):
        self.assertRaises(Exception, self.defaulted.logger)
        self.assertEqual(self.rs.logger(), pz.Logger())

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
        self.rv        = pz.runtime.RuntimeView()
        self.rs        = self.rv.my_resource_set()
