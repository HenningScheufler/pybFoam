import os
from conans import ConanFile, tools


class fmu4foam(ConanFile):
    requires = ["pybind11/2.7.0","OpenFOAMGen/0.2@myuser/OpenFOAMGen"]
    generators = "OpenFOAMGen"

    def imports(self):
        self.copy("*.hpp", "lnInclude", "include")
        self.copy("*.h", "lnInclude", "include")