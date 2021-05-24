import clang.cindex
import os
s = '''
int fac(int n) {
    return (n>1) ? n*fac(n-1) : 1;
}
'''
if 'CLANG_LIBRARY_PATH' in os.environ:
    clang.cindex.Config.set_library_path(os.environ['CLANG_LIBRARY_PATH'])
else:
    clang.cindex.Config.set_library_file('C:/Program Files/LLVM/bin/libclang.dll')
idx = clang.cindex.Index.create()

# each source
tu = idx.parse('tmp.cpp', args=['-std=c++11'],
                unsaved_files=[('tmp.cpp', s)],  options=0)
if not tu:
        #error("unable to load input")
for t in tu.get_tokens(extent=tu.cursor.extent):
    print(t.kind, t.location, t.spelling, t.cursor, t.extent)
    print(t.kind, t.location, t.spelling)
