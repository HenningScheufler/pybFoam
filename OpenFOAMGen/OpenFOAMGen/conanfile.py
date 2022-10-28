from conans import ConanFile
from conans.model import Generator
from pathlib import Path

class OpenFOAMGen(Generator):

    opt_temp_path = "Make/options.template"

    def found_option_template(self):
        return Path(OpenFOAMGen.opt_temp_path).exists()

    @property
    def filename(self):
        if (self.found_option_template()):
            return "Make/options"
        return "Make/defines"

    @property
    def content(self):
        opt_temp = ""
        if self.found_option_template():
            opt_temp += Path(OpenFOAMGen.opt_temp_path).read_text()


        incl = ""
        if not self.found_option_template():
            incl += "CONAN_INCS := \\ \n"
        for i in self.deps_build_info.include_paths:
            incl += "   -I{} \\\n".format(i)

        if (incl):
            incl = incl[:-3]

        lib_incl = ""
        if not self.found_option_template():
            lib_incl += "CONAN_LIBS := \\ \n"
        for lp in self.deps_build_info.lib_paths:
            lib_incl += "   -L{} \\ \n".format(lp)

        for l in self.deps_build_info.libs:
            lib_incl += "   -l{} \\\n".format(l)

        if (lib_incl):
            lib_incl = lib_incl[:-3]


        if self.found_option_template():
            opt_temp = opt_temp.replace("{{{CONAN_INCS_INCS}}}",incl)
            opt_temp = opt_temp.replace("{{{CONAN_INCS_LIBS}}}",lib_incl)
        else:
            opt_temp = incl + "\n\n " + lib_incl + "\n"

        return opt_temp


class OpenFOAMGeneratorPackage(ConanFile):
    name = "OpenFOAMGen"
    version = "0.2"
    url = "https://github.com/HenningScheufler/OpenFOAMGen.git"
    license = "GPLv3"
    description = "OpenFOAM Generator"