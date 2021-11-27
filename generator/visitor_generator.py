import os
import sys

from datetime import datetime
from parse_types import Structure, Enumeration


class CodeGenerator(object):
    """Base code generator."""

    INDENT_SIZE = 4


class VisitorGenerator(CodeGenerator):
    """Code generator for the Reflection Visitor annotation.
    Generates enum visitors, struct type visitors, struct instance visitors, and struct type tuples.
    """

    ANNOTATION = "PROTO_GEN: Reflection Visitor"

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
        self._output("#include <proto/proto_base.hpp>")
        self._output("")
        self._output(f"namespace {self._namespace} {{")
        self._output("")

    def _generate_postamble(self):
        self._output(f"}} // namespace {self._namespace}")

    @staticmethod
    def _get_all_public_fields(s: Structure):
        """Get all public fields, including inherited ones, recursively."""
        field_list = {}
        for b in s.base_classes.values():
            if b is not None:
                field_list.update(VisitorGenerator._get_all_public_fields(b))
        field_list.update(s.public_fields)
        return field_list

    def _generate_tuple_alias(self, s: Structure):
        typestr = ", ".join([field_struct.typename for _, field_struct in self._get_all_public_fields(s).items()])
        self._output("template<>")
        self._output(f"struct TupleType<{s.typename}> {{")
        with IndentBlock(self):
            self._output(f"using type = std::tuple<{typestr}>;")
        self._output("};")
        self._output("")

    def generate_struct_visitor(self, s: Structure):
        self._output("////////////////////////////////////////////////////////////")
        self._output(f"// {s.typename}")
        self._output("////////////////////////////////////////////////////////////")
        self._output("")
        self._generate_tuple_alias(s)
        # instance visitor
        # Use SFINAE template to generate const and non-const ref "overloads"
        self._output(
            f"template <typename Visitor, typename T, std::enable_if_t<std::is_same_v<{s.typename}, std::remove_const_t<T>>, bool> = true>"
        )
        self._output(f"constexpr void visit(T& toVisit, Visitor&& visitor) {{")
        with IndentBlock(self):
            for name, base in s.base_classes.items():
                # don't force the base class to have been annotated for visitation
                if base is not None:
                    self._output(f"visit(static_cast<{base.typename}&>(toVisit), visitor);")
                else:
                    self._output(f"// not visiting unannotated base class {name}")
            for field_name in s.public_fields:
                self._output(f'visitor("{field_name}", toVisit.{field_name});')
        self._output("}")
        self._output("")

        # type visitor
        # specialization goes in the detail namespace as there is a wrapper function to perform type deduction
        self._output("namespace detail {")
        self._output("template <typename Visitor>")
        self._output(f"struct Acceptor<{s.typename}, Visitor> {{")
        with IndentBlock(self):
            self._output("static constexpr void visitType(Visitor&& visitor) {")
            with IndentBlock(self):
                for name, base in s.base_classes.items():
                    # don't force the base class to have been annotated for visitation
                    if base is not None:
                        self._output(f"visit<{base.typename}>(visitor);")
                    else:
                        self._output(f"// not visiting unannotated base class {name}")
                for field_name, field_struct in s.public_fields.items():
                    self._output(f'visitor("{field_name}", TagType<{field_struct.typename}>{{}});')
            self._output("}")
        self._output("};")
        self._output("} // namespace detail")
        self._output("")

    def generate_enum_visitor(self, e: Enumeration):
        self._output("////////////////////////////////////////////////////////////")
        self._output(f"// {e.name}")
        self._output("////////////////////////////////////////////////////////////")
        self._output("")

        # specialization goes in the detail namespace as there is a wrapper function to perform type deduction
        self._output("namespace detail {")
        self._output("template <typename Visitor>")
        self._output(f"struct Acceptor<{e.name}, Visitor> {{")
        with IndentBlock(self):
            self._output("static constexpr void visitType(Visitor&& visitor) {")
            with IndentBlock(self):
                for name, val in e.enumerators.items():
                    # scoped names work for accessing unscoped enum elements too
                    self._output(f'visitor({e.name}::{name}, "{name}", std::underlying_type_t<{e.name}>{{{val}}});')
            self._output("}")
        self._output("};")
        self._output("} // namespace detail")
        self._output("")


class IndentBlock:
    """Generates an indented block when used as a context manager within a CodeGenerator."""

    def __init__(self, generator: CodeGenerator):
        self._generator = generator

    def __enter__(self):
        self._generator.indent_level += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self._generator.indent_level -= 1
