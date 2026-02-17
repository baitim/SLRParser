import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class SLRParserRecipe(ConanFile):
    name = "slr_parser"
    version = "1.0"
    user = "baitim"

    license = ""
    author = ""
    url = ""
    description = "slr_parser"
    topics = ""

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    requires = "flex/2.6.4"
    test_requires = "gtest/1.15.0"

    exports_sources = "CMakeLists.txt", "SLRParser/*"

    def configure(self):
        if self.settings.compiler == "msvc":
            self.settings.compiler.cppstd = "20"
        elif "gnu" in str(self.settings.compiler):
            self.settings.compiler.cppstd = "gnu20"
        else:
            self.settings.compiler.cppstd = "20"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

        # Delete CMakeUserPresets.json
        cmake_user_presets = os.path.join(self.source_folder, "CMakeUserPresets.json")
        if os.path.exists(cmake_user_presets):
            os.remove(cmake_user_presets)

    def build(self):
        os.environ["CONAN_PACKAGE"] = "1"
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "config")
        self.cpp_info.set_property("cmake_file_name", "SLRParser")
        self.cpp_info.set_property("cmake_target_name", "SLRParser::SLRParser")
        self.cpp_info.libs = ["SLRParser"]
        self.cpp_info.includedirs = ["include"]