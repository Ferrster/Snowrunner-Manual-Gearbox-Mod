import os
from conan import ConanFile


class SMGM(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("boost/1.80.0")
        self.requires("fmt/10.0.0")
        self.requires("spdlog/1.11.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.23.5")
