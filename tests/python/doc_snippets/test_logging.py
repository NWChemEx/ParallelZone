import parallelzone as pz
import unittest

class TestLoggerTestCase(unittest.TestCase):

    def test_logging(self):
        rv = pz.runtime.RuntimeView()
        my_rs = rv.my_resource_set()

        log = pz.runtime.RuntimeView().logger()
        severity = pz.Logger.severity
        severities = [severity.trace,
                      severity.debug,
                      severity.info,
                      severity.warn,
                      severity.error,
                      severity.critical] 

        log.set_severity(severity.critical)

        for level in severities:
            log.log(level, "Hello")

        log.set_severity(severity.trace)

        for level in severities:
            log.log(level, "Hello")

        self.assertIsNotNone(my_rs)
