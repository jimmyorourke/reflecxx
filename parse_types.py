from typing import Optional


class Structure:
    def __init__(self, typename: str, annotation: Optional[str] = None):
        self.typename = typename
        self.private_fields = {}
        self.protected_fields = {}
        self.public_fields = {}
        self.annotation = annotation
        self.base_classes = []


class Enumeration:
    def __init__(self, name: str, annotation: Optional[str] = None):
        self.name = name
        # list of pairs of unqualified name, value
        self.enumerators = []
        self.annotation = annotation
