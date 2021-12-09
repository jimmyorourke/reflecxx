This folder contains a patched `cindex.py` libclang python bindings entry point which is used by this project. The bindings from both the [main LLVM repo](https://github.com/llvm/llvm-project/tree/main/clang/bindings/python) and the [legacy clang repo](https://github.com/llvm-mirror/clang/tree/master/bindings/python) which is still the source for the [PyPI package](https://pypi.org/project/clang/) are out of date and missing certain cursor types.

I guess that means I need to submit a patch. Or maybe they need to start using code generation :).
