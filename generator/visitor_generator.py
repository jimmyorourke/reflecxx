# Copyright (c) 2021-2022 Jimmy O'Rourke
# Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
# Official repository: https://github.com/jimmyorourke/reflecxx

import os
import sys

from datetime import datetime
from parse_types import Structure, Enumeration


class VisitorGenerator():
    """Code generator for the Reflection Visitor annotation.
    Generates enum visitors, struct type visitors, struct instance visitors, and struct type tuples.
    """

    ANNOTATION = "REFLECXX_GEN: Reflection Visitor"
    INDENT_SIZE = 4

    def __init__(self, output_file: os.PathLike = None, namespace="generated"):
        self._output_file = output_file
        self._namespace = namespace
        self.indent_level = 0
        self._output_file_handle = None

    def __enter__(self):
        self._open_file()
        self._generate_preamble()
        return self

    def _open_file(self):
        self._output_file_handle = open(self._output_file, "w") if self._output_file is not None else sys.stdout

    def _close_file(self):
        if self._output_file_handle is not sys.stdout:
            self._output_file_handle.close()

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._generate_postamble()
        self._close_file()

    def _output(self, text: str):
        """Outputs input text with a newline at current indent level"""
        indent = " " * self.INDENT_SIZE * self.indent_level
        print(f"{indent}{text}", file=self._output_file_handle)

    def _generate_preamble(self):
        self._output("#pragma once\n")
        self._output(f"// Autogenerated at {datetime.now()} by {__file__}.")
        self._output("// Do not edit, changes will be overwritten!\n")
        self._output("#include <type_traits>")
        self._output("")
        self._output("#include <reflecxx/types.hpp>")
        self._output("")
        self._output(f"namespace {self._namespace} {{")
        self._output("")

    def _generate_postamble(self):
        self._output(f"}} // namespace {self._namespace}")

    def generate_meta_struct(self, s: Structure):
        self._output("////////////////////////////////////////////////////////////")
        self._output(f"// {s.qualified_typename}")
        self._output("////////////////////////////////////////////////////////////")
        self._output("")

        self._output("template <>")
        self._output(f"struct MetaStructInternal<{s.qualified_typename}> {{")
        with IndentBlock(self):
            self._output(f"using Type = {s.qualified_typename};")
            self._output(f'static constexpr std::string_view name{{"{s.name}"}};')
            self._output("static constexpr auto publicFields = std::make_tuple(")
            with IndentBlock(self):
                # make_tuple doesn't allow trailing commas so we have to keep track
                size = len(s.public_fields)
                count = 1
                for field_name, field_struct in s.public_fields.items():
                    suffix = "," if count < size else ""
                    count += 1
                    self._output(f'ClassMember<Type, {field_struct.qualified_typename}>{{&Type::{field_name}, "{field_name}"}}{suffix}')
            self._output(");")

            self._output("static constexpr auto baseClasses = std::make_tuple(")
            with IndentBlock(self):
                size = sum(1 for base in s.base_classes.values() if base is not None)
                count = 1
                for name, base in s.base_classes.items():
                    # don't force the base class to have been annotated for visitation
                    if base is not None:
                        suffix = "," if count < size else ""
                        count += 1
                        self._output(f"type_tag<{base.qualified_typename}>{{}}{suffix}")
                    else:
                        self._output(f"// skipping unannotated base class {name}")
            self._output(");")
        self._output("};")
        self._output("")

    def generate_meta_enum(self, e: Enumeration):
        self._output("////////////////////////////////////////////////////////////")
        self._output(f"// {e.qualified_name}")
        self._output("////////////////////////////////////////////////////////////")
        self._output("")

        self._output("template <>")
        self._output(f"struct MetaEnumInternal<{e.qualified_name}> {{")
        with IndentBlock(self):
            self._output(f"using Utype = std::underlying_type_t<{e.qualified_name}>;")
            self._output(f'static constexpr std::string_view name{{"{e.name}"}};')
            size = len(e.enumerators)
            self._output(f"static constexpr std::array<Enumerator<{e.qualified_name}>, {size}> enumerators = {{{{")
            with IndentBlock(self):
                for name, val in e.enumerators.items():
                    # scoped names work for accessing unscoped enum elements too
                    self._output(f'{{{e.qualified_name}::{name}, "{name}", Utype{{{val}}}}},')
            self._output("}};")
        self._output("};")
        self._output("")

class IndentBlock:
    """Generates an indented block when used as a context manager within a VisitorGenerator."""

    def __init__(self, generator: VisitorGenerator):
        self._generator = generator

    def __enter__(self):
        self._generator.indent_level += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._generator.indent_level -= 1
