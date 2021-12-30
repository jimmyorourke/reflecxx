#===- enumerations.py - Python Enumerations ------------------*- python -*--===#
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===------------------------------------------------------------------------===#

"""
Clang Enumerations
==================
This module provides static definitions of enumerations that exist in libclang.
Enumerations are typically defined as a list of tuples. The exported values are
typically munged into other types or classes at module load time.
All enumerations are centrally defined in this file so they are all grouped
together and easier to audit. And, maybe even one day this file will be
automatically generated by scanning the libclang headers!
"""

# Maps to CXTokenKind. Note that libclang maintains a separate set of token
# enumerations from the C++ API.
TokenKinds = [
    ('PUNCTUATION', 0),
    ('KEYWORD', 1),
    ('IDENTIFIER', 2),
    ('LITERAL', 3),
    ('COMMENT', 4),
]

__all__ = ['TokenKinds']