"""Classes representing the types parsed by libclang."""

from typing import Dict, Optional, Union


class Structure:
    """Represents a C or C++ Struct or Class."""

    def __init__(self, typename: str, annotation: Optional[str] = None):
        self.typename = typename
        # name to Structure
        self.private_fields: Dict[str, "Structure"] = {}
        self.protected_fields: Dict[str, "Structure"] = {}
        self.public_fields: Dict[str, "Structure"] = {}
        # name to Structure if base is reflected, else name to None
        self.base_classes: Dict[str, Union["Structure", None]] = {}
        self.annotation: str = annotation


class Enumeration:
    """Represents a C++ scoped enum or a C or C++ unscoped enum."""

    def __init__(self, name: str, annotation: Optional[str] = None):
        self.name: str = name
        # unqualified name to value mappings
        self.enumerators: Dict[str, int] = {}
        self.annotation: str = annotation
