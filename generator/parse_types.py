# Copyright (c) 2021-2022 Jimmy O'Rourke
# Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
# Official repository: https://github.com/jimmyorourke/reflecxx

"""Classes representing the types parsed by libclang."""

from typing import Dict, Optional, Union


class Structure:
    """Represents a C or C++ Struct or Class."""

    def __init__(self, qualified_typename: str, name: Optional[str] = None, annotation: Optional[str] = None):
        self.qualified_typename: str = qualified_typename
        self.name: str = name if name else qualified_typename
        # name to Structure
        self.private_fields: Dict[str, "Structure"] = {}
        self.protected_fields: Dict[str, "Structure"] = {}
        self.public_fields: Dict[str, "Structure"] = {}
        # name to Structure if base is reflected, else name to None
        self.base_classes: Dict[str, Union["Structure", None]] = {}
        self.annotation: str = annotation


class Enumeration:
    """Represents a C++ scoped enum or a C or C++ unscoped enum."""

    def __init__(self, qualified_name: str, name: Optional[str] = None, annotation: Optional[str] = None):
        self.qualified_name: str = qualified_name
        self.name: str = name if name else qualified_name
        # unqualified name to value mappings
        self.enumerators: Dict[str, int] = {}
        self.annotation: str = annotation
