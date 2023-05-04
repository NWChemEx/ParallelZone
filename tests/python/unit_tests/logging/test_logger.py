import parallelzone as pz
import unittest

class LoggerTestCase(unittest.TestCase):
    def setUp(self):
        self.defaulted = pz.Logger()
        self.log       = pz.runtime.RuntimeView().logger()

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pz.Logger())
        self.assertFalse(self.defaulted != pz.Logger())

        self.assertNotEqual(self.defaulted, self.log)
        self.assertFalse(self.defaulted == self.log)
