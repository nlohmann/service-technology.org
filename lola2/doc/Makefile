.PHONY : _cppcheck

_style:
    astyle ../src/*.cc ../src/*.c ../src/*.h --options=astylerc --recursive

_cppcheck:
    cppcheck --enable=all ../src/*.cc ../src/*.c ../src/*.h

