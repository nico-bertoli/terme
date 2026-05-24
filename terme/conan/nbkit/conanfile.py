from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.scm import Git


class NbkitConan(ConanFile):
    name = "nbkit"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    package_type = "static-library"

    def source(self):
        Git(self).clone("https://github.com/nico-bertoli/nbkit.git", target=".")

    def requirements(self):
        self.requires("magic_enum/0.8.0", transitive_headers=True)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = "OFF" 
        tc.generate()
        CMakeDeps(self).generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        CMake(self).install()

    def package_info(self):
        self.cpp_info.libs = ["nbkit"]
        self.cpp_info.set_property("cmake_file_name", "nbkit")
        self.cpp_info.set_property("cmake_target_name", "nbkit::nbkit")
