import argparse
import sys

from os import PathLike
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


def main(libclang_directory:PathLike, input_files:List[PathLike], output_file:PathLike, flags:List[str], namespace:str):

    # clang.cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib/')
    # clang.cindex.Config.set_library_path("C:\\Program Files\\LLVM\\bin")
    clang.cindex.Config.set_library_path(libclang_directory)
    index = clang.cindex.Index.create()

    # tu = index.parse('tmp.cpp', args=['-std=c++11'], unsaved_files=[('tmp.cpp', s)],  options=0)

    structures = []
    enums = []

    # do we need to be selective about flags?
    # remove any duplicates
    flags = set(flags)
    for flag in flags:
        if flag.startswith('/D'):
            flags.discard(flag)
            flags.add(flag.replace('/D', '-D'))

    # Add flag indicating we are in the generator. This allows avoiding compilation of code that depends on generated
    # code, such as includes of generated headers.
    flags.add('-DPROTO_GENERATION')

    # The only flags we really care about are for this compilation are include paths, compile definitions, and c++ standard. Luckily these flags don't differ much between compilers.
    # clang_flags = set()
    # for flag in flags:
    #     if flag.startswith['-I'] or flag.startswith['-std'] or flag.startswith['-D']:
    #         clang_flags.add(flag)
    #     elif flag.startswith['/D']:
    #         # Accept some MSVC style definitions.
    #         # Even using Ninja generator on Windows produces flags in the compile_commands.json that start with /D, such as /DWIN32, while user specified definitions end up using -D.
    #         clang_flags.add(flag.replace('/D', '-D'))


    print(flags)

    for file in input_files:
        # compile
        # speed up parsing
        options = TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
        tu = TranslationUnit.from_source(file, args=flags, unsaved_files=None, options=options, index=index)

        for diag in tu.diagnostics:
            print("Parse diagostic", diag)
            print(diag.Warning) # we can get warnings from unused commandline args
            if diag.severity > diag.Warning:
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
    # Use a single string rather than a list to be able to support the leading dashes on the flags
    parser.add_argument("--flags", "-f", type=str, default="", help="Compiler flags (including include paths, compile definitions, c++ standard) to provide to libclang, space separated as would appear on the commandline. Needs to be specified as --flags=")
    parser.add_argument("--namespace", "-n", help="Namespace for generated C++ code.", default="proto")
    args = parser.parse_args()

    args.input_files = args.input_files or ["test/test_types.hpp"]
    args.output_file = args.output_file or "out.hpp"

    main(args.libclang_directory, args.input_files, args.output_file, args.flags.split(), args.namespace)
