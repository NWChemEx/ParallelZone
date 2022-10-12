'''
Python Package for unit testing ParallelZone
'''
#TODO: import parallelzone should suffice
from parallelzone import parallelzone
import sys

comm_world = parallelzone.runtime.RuntimeView(len(sys.argv), sys.argv)
