from distutils.core import setup
from distutils.core import Extension

MOD = "UDPclient"
module = Extension(MOD, sources = ["UDPclient.cpp"])
setup(name = MOD, ext_modules = [module])