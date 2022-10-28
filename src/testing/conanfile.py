import os
from conans import ConanFile, tools


class fmu4foam(ConanFile):
    requires = ["catch2/2.13.7","OpenFOAMGen/0.2@myuser/OpenFOAMGen"]
    generators = "OpenFOAMGen"

    def imports(self):
        self.copy("*.hpp", "lnInclude", "include")
        self.copy("*.h", "lnInclude", "include")