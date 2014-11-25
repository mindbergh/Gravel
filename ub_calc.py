#!/usr/bin/env python
# coding=utf-8
from math import pow

def do():
    for i in xrange(1,65):
        print "%0.4f" % (i * (pow(2,1./i)- 1))

if __name__ == "__main__":
    do()
