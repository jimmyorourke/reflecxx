

import clang.cindex
from clang.cindex import CursorKind
from clang.cindex import TranslationUnit
from clang.cindex import AccessSpecifier

from typing import Dict, List, Optional
from datetime import datetime
s = '''
#include <iostream>
#include <stdint.h>
namespace myns{
int fac(int n) {
    return (n>1) ? n*fac(n-1) : 1;
}

//struct [[annotate("my annotation1")]] Foo {
struct Foo {
    int a;
    double b;
};

class Base {
public:
    int baseInt;
};

namespace myinnerns {
class Bar : public Base {
    double c;
    Foo f;
};
//}__attribute__((annotate("my annotation")));
}
}

'''

#clang.cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib/')
clang.cindex.Config.set_library_path('C:\\Program Files\\LLVM\\bin')

index = clang.cindex.Index.create()
# for each source file

#tu = index.parse('tmp.cpp', args=['-std=c++11'],
#                unsaved_files=[('tmp.cpp', s)],  options=0)
# speed up parsing
options = TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
tu = TranslationUnit.from_source('testpp.cpp', args=None, unsaved_files=None, options=options, index=index)
# tu = TranslationUnit.from_source('testpp.cpp', args=['-std=c++2a', '--pedantic'], unsaved_files=None, options=0, index=index)

print(len(tu.diagnostics))
for d in tu.diagnostics:
    print(d)

for diag in tu.diagnostics:
    if diag.severity > diag.Warning:
        print("ERROR", diag)
        exit(1)
    else:
        print(diag)

# parsing_succeeded = True
#   for diag in translation_unit.diagnostics:
#     if diag.severity == diag.Warning:
#       logging.warning(diag)
#     if diag.severity in [diag.Warning, diag.Fatal]:
#       logging.error(diag)
#       parsing_succeeded = False

#   if not parsing_succeeded:
#     logging.fatal('Parsing failed')

#tu.save("tu-out.txt")
#cursorwalk
#cursor.type.spelling
#cursorKind.ANNOTATE_ATTR
# cursor get canonical kind
# recursively traverse the cursors



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


def find_classes(cursor, classes, level):
    print("\t" * level, cursor.spelling, cursor.displayname, cursor.kind)
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        classes.append(cursor)
    if cursor.kind.is_attribute():
        print("attribute")
    for c in cursor.get_children():
        find_classes(c, classes, level+1)

classes = []
#find_classes(tu.cursor, classes, 0)
for c in classes:
    print(c.spelling)

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
                    if c.access_specifier == AccessSpecifier.PROTECTED:
                        structure.protected_fields[c.spelling] = Structure(c.type.spelling)
                    if c.access_specifier == AccessSpecifier.PRIVATE:
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
            c = cursor.enum_type.get_declaration()
            # enum_value for each item self.kind == CursorKind.ENUM_CONSTANT_DECL
            print("\tENUM decl", c.spelling, c.displayname, c.kind, c.type.kind, c.type.spelling, c.type.kind.spelling, c.access_specifier)
            for c in cursor.get_children():
                if c.kind == CursorKind.ENUM_CONSTANT_DECL:
                 print("\t",c.kind, c.spelling, c.type.spelling, c.type.kind, c.type.kind.spelling, c.enum_value)
            c = cursor.type.get_canonical()
            print("\tENUM canon type", c.spelling,c.kind)
            for c in cursor.get_children():
                if c.kind == CursorKind.ENUM_CONSTANT_DECL:
                    print("\t",c.kind, c.spelling, c.type.spelling, c.type.kind, c.type.kind.spelling, c.enum_value)

print("#pragma once")
print(f"// Autogenerated at {datetime.now()}")
print("#include <type_traits>")
for s in structures:
    print(f"template <typename VisitorType, typename T, std::enable_if_t<std::is_same_v<{s.typename}, typename std::remove_const_t<T>>, bool> = true>")
    print(f"constexpr void visit([[maybe_unused]] T& toVisit, [[maybe_unused]] VisitorType&& visitor) {{")
    for base in s.base_classes:
        # don't force the base class to have been annotated for visitation
        # TODO: hash table
        for p in structures:
            if p.typename == base:
                print(f"\tvisit(static_cast<{base}&>(toVisit), visitor);")
                break
        else:
            print(f"\t// not visiting base class {base}")
    for field_name in s.public_fields:
        print(f"\tvisitor(\"{field_name}\", toVisit.{field_name});")
    print("}\n")

for e in enums:
    print(f"Enum type {e.name}")
    for p in e.enumerators:
        # scoped names work for accessing unscoped enum elements too
        print(f"\tvisitpr({e.name}::{p[0]}, \"{p[0]}\", std::underlying_type_t<{e.name}>{{{p[1]}}})")

    #pass
    #print(cursor.spelling, cursor.displayname, cursor.kind)

#CursorKind.STRUCT_DECL
#CursorKind.CLASS_DECL
#CursorKind.FIELD_DECL
#.type.kind, TypeKind.INT


#CursorKind.ENUM_DECL

