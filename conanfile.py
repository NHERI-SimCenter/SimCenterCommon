from conans import ConanFile, tools, VisualStudioBuildEnvironment
import os

class CommonConan(ConanFile):
    name = "SimCenterCommonQt"
    version = "0.1.0"
    license = "BSD"
    author = "Wael Elhaddad (elhaddad@berkeley.edu)"
    url = ""
    description = "SimCenter Common Qt Library"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake", "cmake"
    requires = "jansson/2.11@bincrafters/stable", "libcurl/7.64.1@bincrafters/stable"
    build_policy = "missing"
    
    options = {
        "MDOFwithQt3D": [True, False],
        "withQt":[True, False]
    }


    default_options = {"MDOFwithQt3D": False, "withQt": False}

    scm = {
         "type": "git",
         "url": "auto",
         "revision": "auto"
      }
    

    def configure(self):
        if self.settings.os == "Windows":
           self.options["libcurl"].with_winssl = True
           self.options["libcurl"].with_openssl = False
        
        if self.options.withQt:
           self.options["qt"].qtcharts = True
           self.options["qt"].qt3d = True


    def build_requirements(self):
        if self.settings.os == "Windows":
            self.build_requires("jom_installer/1.1.2@bincrafters/stable")
        
        if self.options.withQt:
            self.build_requires("qt/5.11.3@bincrafters/stable")


    def package_id(self):
        del self.info.options.withQt


    def build(self):
        if self.settings.os == "Windows":
            env_build = VisualStudioBuildEnvironment(self)
            with tools.environment_append(env_build.vars):
                vcvars = tools.vcvars_command(self.settings)
            
            qmake = "%s && qmake" % (vcvars)
            makeCommand = "%s && jom" % (vcvars)
        else:
            qmake = 'qmake'
            makeCommand = 'make'

        qmakeCommand = '%s "CONFIG+=%s" %s/SimCenterCommonQt.pro' % (qmake, self.settings.build_type, self.source_folder)
        if(self.options.MDOFwithQt3D):
            qmakeCommand += ' "DEFINES+=_GRAPHICS_Qt3D"'

        self.run(qmakeCommand, run_environment=True) 
        self.run(makeCommand, run_environment=True) 


    def package(self):
        self.copy("*.h", src="Common", dst="include", keep_path=False)
        self.copy("*.h", src="InputSheetBM", dst="include", keep_path=False)
        self.copy("*.h", src="RandomVariables", dst="include", keep_path=False)
        self.copy("*/*.h", src="Workflow", dst="include", keep_path=False)
        self.copy("*SimCenterCommonQt.lib", dst="lib", keep_path=False)
        self.copy("*SimCenterCommonQt.a", dst="lib", keep_path=False)


    def package_info(self):
        self.cpp_info.libs = ["SimCenterCommonQt"]
