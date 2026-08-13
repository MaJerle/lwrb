set(CMAKE_SYSTEM_NAME   Windows)

# Some default GCC settings
# Use MSYS2's "mingw-w64-i686-gcc" package
set(CMAKE_C_COMPILER    i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER  i686-w64-mingw32-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
