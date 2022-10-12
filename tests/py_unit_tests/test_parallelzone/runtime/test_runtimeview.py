from parallelzone import parallelzone
import unittest
import sys


class TestRuntimeView(unittest.TestCase):

    def setUp(self):
        argc = len(sys.argv)
        self.defaulted = parallelzone.runtime.RuntimeView()
        self.argc_argv = parallelzone.runtime.RuntimeView(argc, sys.argv)

    def test_default_ctor(self):
        self.assertGreater(self.defaulted.size(), 0)
        self.assertEqual(self.defaulted.did_i_start_madness(), False)
