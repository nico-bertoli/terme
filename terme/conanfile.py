from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
import os

class TermeConan(ConanFile):
    name = "terme"
    version = "1.0.0"
    
    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "terme/*", "tests/*"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("nbkit/1.0.0", visible=False) # private dependency
        self.requires("miniaudio/0.11.22")

    def build_requirements(self):
        self.test_requires("gtest/1.15.0") #imported for future use

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = "ON"
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["terme"]