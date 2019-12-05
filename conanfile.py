from conans import ConanFile, tools, VisualStudioBuildEnvironment
import os

class CommonConan(ConanFile):
    name = "SimCenterCommonQt"
    version = "0.0.1"
    license = "BSD"
    author = "Wael Elhaddad (elhaddad@berkeley.edu)"
    url = ""
    description = "SimCenter Common Qt Library"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake", "cmake"
    build_requires = "qt/5.9.8@bincrafters/stable"
    requires = "jansson/2.11@simcenter/stable", "libcurl/7.64.1@bincrafters/stable"
    default_options = {"qt:qtcharts":True, "qt:qt3d":True}
    build_policy = "missing"
    
    def configure(self):
        if self.settings.os == "Windows":
           self.options["libcurl"].with_winssl = True
           self.options["libcurl"].with_openssl = False


    def source(self):
        self.run("git clone --single-branch --branch conan https://github.com/el7addad/SimCenterCommon.git")

    def build(self):
        if self.settings.os == "Windows":
            env_build = VisualStudioBuildEnvironment(self)
            with tools.environment_append(env_build.vars):
                vcvars = tools.vcvars_command(self.settings)
            self.output.info("Using vcvars: %s" % (vcvars))
            self.run("qmake -v", run_environment=True)
            self.run('%s && qmake  "CONFIG+=%s" %s/SimCenterCommon/SimCenterCommonQt.pro' % (vcvars, self.settings.build_type, self.source_folder), run_environment=True)
            self.run("%s && nmake" % (vcvars), run_environment=True)
        else:
            self.run('qmake "CONFIG+=%s" %s/SimCenterCommon/SimCenterCommonQt.pro' % (self.settings.build_type, self.source_folder), run_environment=True)
            self.run("make", run_environment=True)
            
    def package(self):
        self.copy("*.h", src="SimCenterCommon/Common", dst="include", keep_path=False)
        self.copy("*.h", src="SimCenterCommon/InputSheetBM", dst="include", keep_path=False)
        self.copy("*.h", src="SimCenterCommon/RandomVariables", dst="include", keep_path=False)
        self.copy("*/*.h", src="SimCenterCommon/Workflow", dst="include", keep_path=False)
        self.copy("*SimCenterCommonQt.lib", dst="lib", keep_path=False)
        self.copy("*SimCenterCommonQt.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["SimCenterCommonQt"]

