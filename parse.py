

import clang.cindex
from clang.cindex import CursorKind
from clang.cindex import TranslationUnit
from clang.cindex import AccessSpecifier

from typing import Dict, List, Optional
from datetime import datetime

import sys
#clang.cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib/')
clang.cindex.Config.set_library_path('C:\\Program Files\\LLVM\\bin')

index = clang.cindex.Index.create()
# for each source file

#tu = index.parse('tmp.cpp', args=['-std=c++11'],
#                unsaved_files=[('tmp.cpp', s)],  options=0)
# speed up parsing
options = TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
#infile = 'testpp.cpp'
infile = 'test/test_types.hpp'
tu = TranslationUnit.from_source(infile, args=None, unsaved_files=None, options=options, index=index)

for diag in tu.diagnostics:
    print("Parse diagostic", diag)
    if diag.severity >= diag.Warning:
        print("Code genration failed")
        exit(1)

class Structure:

    def __init__(self, typename:str, annotation:Optional[str]=None):
        self.typename = typename
        self.private_fields = {}
        self.protected_fields = {}
        self.public_fields = {}
        self.annotation = annotation
        self.base_classes = []

class Enumeration:

    def __init__(self, name:str, annotation:Optional[str]=None):
        self.name = name
        # list of pairs of unqualified name, value
        self.enumerators = []
        self.annotation = annotation


# given a cursor, walks its dierct children to determine if there is an annotation attribute. If so returns the cursor
def find_annotate_attr(cursor):
    for c in cursor.get_children():
        if c.kind == CursorKind.ANNOTATE_ATTR:
            return c
    return None

structures = []
enums = []

for cursor in tu.cursor.walk_preorder():
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        #print("found class", cursor.spelling, cursor.displayname, cursor.kind)
        attr = find_annotate_attr(cursor)
        if attr is not None:
            print("found annotated class", cursor.spelling, cursor.displayname, cursor.kind, cursor.type.kind, cursor.type.spelling, cursor.access_specifier)
            print(attr.spelling, attr.displayname, attr.kind)

            structure = Structure(cursor.type.spelling, attr.spelling)
            structures.append(structure)

            print("fields")
            for c in cursor.get_children():
            #for c in cursor.walk_preorder():

                if c.kind == CursorKind.FIELD_DECL:
                    #c.canonical()
                    print("\t", c.spelling, c.displayname, c.kind, c.type.kind, c.type.spelling, c.access_specifier)
                    if c.access_specifier == AccessSpecifier.PUBLIC:
                        structure.public_fields[c.spelling] = Structure(c.type.spelling)
                    elif c.access_specifier == AccessSpecifier.PROTECTED:
                        structure.protected_fields[c.spelling] = Structure(c.type.spelling)
                    elif c.access_specifier == AccessSpecifier.PRIVATE:
                        structure.private_fields[c.spelling] = Structure(c.type.spelling)
                if c.kind == CursorKind.CXX_BASE_SPECIFIER:
                    # class is derived
                    print("base\t", c.spelling, c.displayname, c.kind, c.type.kind, c.type.spelling, c.access_specifier)
                    structure.base_classes.append(c.type.spelling)
                else:
                    print("FOUND ANNOTATED", c.kind)

    if cursor.kind == CursorKind.ENUM_DECL:
        attr = find_annotate_attr(cursor)
        if attr is not None:
            c = cursor
            print("annotated ENUM", c.spelling, c.displayname, c.kind, c.type.kind, c.type.spelling, c.type.kind.spelling,c.enum_type.kind, c.enum_type.kind.spelling)
            enum = Enumeration(cursor.type.spelling, attr.spelling)
            enums.append(enum)
            for c in cursor.get_children():
                if c.kind == CursorKind.ENUM_CONSTANT_DECL:
                    # c.spelling, c.enum_value
                    enum.enumerators.append((c.spelling, c.enum_value))
                    print("\t",c.kind, c.spelling, c.type.spelling, c.type.kind, c.type.kind.spelling, c.enum_value)

class IndentBlock:

    def __init__(self, generator):
        self._generator = generator

    def __enter__(self):
        self._generator.indent_level += 1
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
         self._generator.indent_level -= 1

class VisitorGenerator:
    indent_size = 4

    def __init__(self, output_file=None, namespace="generated"):
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

    def _output(self, text:str):
        """Outputs input text with a newline at current indent level"""
        indent = " " * self.indent_size * self.indent_level
        print(f"{indent}{text}", file=self._output_file_handle)

    def _generate_preamble(self):
        self._output("#pragma once\n")
        self._output(f"// Autogenerated at {datetime.now()} by {__file__}.")
        self._output("// Do not edit, changes will be overwritten!\n")
        self._output("#include <type_traits>")
        self._output("")
        self._output(f"namespace {self._namespace} {{")
        self._output("")


    def _generate_postamble (self):
        self._output(f"}} // namespace {self._namespace}")

    def generate_struct_visitor(self, s):
        # Use SFINAE template to generate const and non-const ref "overloads"
        self._output(f"template <typename VisitorType, typename T, std::enable_if_t<std::is_same_v<{s.typename}, std::remove_const_t<T>>, bool> = true>")
        self._output(f"constexpr void visit([[maybe_unused]] T& toVisit, [[maybe_unused]] VisitorType&& visitor) {{")
        with IndentBlock(self):
            for base in s.base_classes:
                # don't force the base class to have been annotated for visitation
                # TODO: hash table
                for p in structures:
                    if p.typename == base:
                        self._output(f"visit(static_cast<{base}&>(toVisit), visitor);")
                        break
                else:
                    self._output(f"// not visiting unannotated base class {base}")
            for field_name in s.public_fields:
                self._output(f"visitor(\"{field_name}\", toVisit.{field_name});")
        self._output("}")
        self._output("")

    def generate_enum_visitor(self, e):
        self._output("template <typename Visitor>")
        self._output(f"struct Acceptor<{e.name}, Visitor> {{")
        with IndentBlock(self):
            self._output("static constexpr void visit(Visitor&& visitor) {")
            with IndentBlock(self):
                for p in e.enumerators:
                    # scoped names work for accessing unscoped enum elements too
                    self._output(f"visitor({e.name}::{p[0]}, \"{p[0]}\", std::underlying_type_t<{e.name}>{{{p[1]}}});")
            self._output("}")
        self._output("};")
        self._output("")


with VisitorGenerator("out.hpp") as v:
    for s in structures:
        v.generate_struct_visitor(s)
    for e in enums:
        v.generate_enum_visitor(e)

#something needs to emit or link aganst and include
# template <typename SFINAE_ENUM_t, typename Visitor>
# struct Acceptor;

# template <typename EnumType, typename VisitorType>
# constexpr void visit(VisitorType&& visitor) {
#   Acceptor<EnumType, VisitorType>::visit(std::forward<VisitorType>(visitor));
# }


#CursorKind.ENUM_DECL

