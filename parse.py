import argparse
import sys

from typing import Dict, List, Optional

import clang.cindex
from clang.cindex import CursorKind
from clang.cindex import TranslationUnit
from clang.cindex import AccessSpecifier

from parse_types import Structure, Enumeration
from visitor_generator import VisitorGenerator


# given a cursor, walks its dierct children to determine if there is an annotation attribute. If so returns the cursor
def find_annotate_attr(cursor):
    for c in cursor.get_children():
        if c.kind == CursorKind.ANNOTATE_ATTR:
            return c
    return None


def check_annotated_struct(cursor, structures):
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        attr = find_annotate_attr(cursor)
        if attr is not None:
            structure = Structure(cursor.type.spelling, attr.spelling)
            structures.append(structure)
            for c in cursor.get_children():
                if c.kind == CursorKind.FIELD_DECL:
                    if c.access_specifier == AccessSpecifier.PUBLIC:
                        structure.public_fields[c.spelling] = Structure(c.type.spelling)
                    elif c.access_specifier == AccessSpecifier.PROTECTED:
                        structure.protected_fields[c.spelling] = Structure(c.type.spelling)
                    elif c.access_specifier == AccessSpecifier.PRIVATE:
                        structure.private_fields[c.spelling] = Structure(c.type.spelling)
                if c.kind == CursorKind.CXX_BASE_SPECIFIER:
                    # class is derived
                    structure.base_classes.append(c.type.spelling)


def check_annotated_enum(cursor, enums):
    if cursor.kind == CursorKind.ENUM_DECL:
        attr = find_annotate_attr(cursor)
        if attr is not None:
            enum = Enumeration(cursor.type.spelling, attr.spelling)
            enums.append(enum)
            for c in cursor.get_children():
                if c.kind == CursorKind.ENUM_CONSTANT_DECL:
                    enum.enumerators.append((c.spelling, c.enum_value))


def main(libclang_directory, input_files, output_file, flags, namespace):

    # clang.cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib/')
    # clang.cindex.Config.set_library_path("C:\\Program Files\\LLVM\\bin")
    clang.cindex.Config.set_library_path(libclang_directory)
    index = clang.cindex.Index.create()

    # tu = index.parse('tmp.cpp', args=['-std=c++11'], unsaved_files=[('tmp.cpp', s)],  options=0)

    structures = []
    enums = []

    for file in input_files:
        # compile
        # speed up parsing
        options = TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
        tu = TranslationUnit.from_source(file, args=flags, unsaved_files=None, options=options, index=index)

        for diag in tu.diagnostics:
            print("Parse diagostic", diag)
            if diag.severity >= diag.Warning:
                print("Code genration failed")
                exit(1)

        for cursor in tu.cursor.walk_preorder():
            check_annotated_struct(cursor, structures)
            check_annotated_enum(cursor, enums)

    with VisitorGenerator(output_file=output_file, namespace=namespace) as v:
        v.generate_struct_visitors(structures)
        v.generate_enum_visitors(enums)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--libclang-directory", "-c", help="libclang shared object directory", default="C:\\Program Files\\LLVM\\bin"
    )
    parser.add_argument("--input-files", "-i", nargs="*", help="Input source file(s) to process.")
    parser.add_argument("--output-file", "-o", help="File name for generated output")
    parser.add_argument("--flags", "-f", help="Compiler flags to provide to libclang")
    parser.add_argument("--namespace", "-n", help="Namespace for generated C++ code.", default="proto_generated")
    args = parser.parse_args()

    args.input_files = args.input_files or ["test/test_types.hpp"]
    args.output_file = args.output_file or "out.hpp"

    main(args.libclang_directory, args.input_files, args.output_file, args.flags, args.namespace)
