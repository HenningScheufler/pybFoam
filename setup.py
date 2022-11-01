import os
from glob import glob
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from pybind11.setup_helpers import Pybind11Extension, build_ext

cwd = os.getcwd()

class WMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        print("Compiling Library")
        subprocess.call(os.path.join(cwd,"src","pybFoam","Allwmake"))
        os.chdir(cwd)

class wmake_build(build_ext):
    def run(self):
        print("run")
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        print("build_extension")

ext_modules = [
    Pybind11Extension(
        "pybFoam",
        sorted(glob("src/pybFoam/src/*.C")),
    ),
]

setup()
# setup(ext_modules = [WMakeExtension("pybFoam", sorted(glob("src/pybFoam/src/*.C")))],
#       cmdclass = {'build_ext': wmake_build},
#       )





