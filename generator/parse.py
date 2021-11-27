import argparse
import sys
import os

from os import PathLike
from pathlib import Path
from typing import Dict, List, Optional

import clang.cindex
from clang.cindex import Cursor
from clang.cindex import CursorKind
from clang.cindex import TranslationUnit
from clang.cindex import AccessSpecifier

from parse_types import Structure, Enumeration
from visitor_generator import VisitorGenerator


def find_annotate_attr(cursor: Cursor) -> Optional[Cursor]:
    """Given a cursor, walks its direct children to determine if there is an annotation attribute. If so, and it is a
    PROTO_GEN annotation, returns the cursor."""
    for c in cursor.get_children():
        if c.kind == CursorKind.ANNOTATE_ATTR and "PROTO_GEN" in c.spelling:
            return c
    return None


def check_annotated_struct(cursor: Cursor, structures: Dict[str, Structure]) -> None:
    """Checks if a cursor is an annotated struct or class declaration. If so, parses it and ands it to the structures
    dict."""
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        attr = find_annotate_attr(cursor)
        if attr is not None:
            structure = Structure(cursor.type.spelling, attr.spelling)
            structures[cursor.type.spelling] = structure
            for c in cursor.get_children():
                if c.kind == CursorKind.FIELD_DECL:
                    if c.access_specifier == AccessSpecifier.PUBLIC:
                        # Use canonical type to handle things like:
                        ## namespace ns {
                        ## struct S{};
                        ## std::array<S, 3> Arr;
                        ## }
                        # The canonical type for Arr will include namespace scoping, ie std::array<ns::S, 3>.
                        # This way the generated code functions properly when it is not in namespace ns.
                        structure.public_fields[c.spelling] = Structure(c.type.get_canonical().spelling)
                    elif c.access_specifier == AccessSpecifier.PROTECTED:
                        structure.protected_fields[c.spelling] = Structure(c.type.get_canonical().spelling)
                    elif c.access_specifier == AccessSpecifier.PRIVATE:
                        structure.private_fields[c.spelling] = Structure(c.type.get_canonical().spelling)
                if c.kind == CursorKind.CXX_BASE_SPECIFIER:
                    # class is derived
                    # Set the derived object to None for now until we can determine if it has been relfected
                    structure.base_classes[c.type.get_canonical().spelling] = None


def check_annotated_enum(cursor: Cursor, enums: List[Enumeration]) -> None:
    """Checks if a cursor is an annotated enum declaration. If so, parses it and ands it to the enums list."""
    if cursor.kind == CursorKind.ENUM_DECL:
        attr = find_annotate_attr(cursor)
        if attr is not None:
            enum = Enumeration(cursor.type.spelling, attr.spelling)
            enums.append(enum)
            for c in cursor.get_children():
                if c.kind == CursorKind.ENUM_CONSTANT_DECL:
                    enum.enumerators[c.spelling] = c.enum_value


def main(
    libclang_directory: PathLike, input_files: List[PathLike], output_folder: PathLike, flags: List[str], namespace: str
):
    clang.cindex.Config.set_library_path(libclang_directory)
    index = clang.cindex.Index.create()

    flags.append("-DPROTO_GENERATION")

    for file in input_files:
        # Dict of name to Structure. Use a dict so after parsing all annotated structures we can efficiently look up whether
        # base classes were annotated or not.
        structures = {}
        enums = []

        # compile
        # speed up parsing
        options = TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
        tu = TranslationUnit.from_source(file, args=flags, unsaved_files=None, options=options, index=index)

        for diag in tu.diagnostics:
            # TODO: log level?
            print("Parse diagostic", diag)
            # we can get warnings from unused commandline args
            if diag.severity > diag.Warning:
                print("Code generation failed.")
                print("Flags:", flags)
                exit(1)

        for cursor in tu.cursor.walk_preorder():
            check_annotated_struct(cursor, structures)
            check_annotated_enum(cursor, enums)

        # With all structures parsed, base classes can be pointed to if they were annotated.
        for s in structures.values():
            for base_class in s.base_classes:
                if base_class in structures:
                    s.base_classes[base_class] = structures[base_class]

        # Generate!
        os.makedirs(output_folder, exist_ok=True)
        output_file = Path(output_folder) / (Path(file).stem + "_proto_generated.hpp")

        with VisitorGenerator(output_file=output_file, namespace=namespace) as v:
            for s in structures.values():
                if s.annotation == v.ANNOTATION:
                    v.generate_struct_visitor(s)
            for e in enums:
                if e.annotation == v.ANNOTATION:
                    v.generate_enum_visitor(e)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--libclang-directory", "-c", help="libclang shared object directory", default="C:\\Program Files\\LLVM\\bin"
    )
    parser.add_argument("--input-files", "-i", nargs="*", help="Input source file(s) to process.")
    parser.add_argument("--output-folder", "-o", help="Folder for generated output", default=".")
    # Use a single string rather than a list to be able to support the leading dashes on the flags
    parser.add_argument(
        "--flags",
        "-f",
        type=str,
        help="Compiler flags (including include paths, compile definitions, c++ standard) to provide to libclang, space"
        " separated as would appear on the commandline. Needs to be specified as --flags=",
        default="",
    )
    args = parser.parse_args()
    print(args)
    # since we're going to be specializing some templates, we have to use the same namespace as the original
    # declarations
    namespace = "proto"
    main(args.libclang_directory, args.input_files, args.output_folder, args.flags.split(), namespace)
