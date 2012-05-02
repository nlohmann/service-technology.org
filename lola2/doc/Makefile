.PHONY : _cppcheck

_style:
    for FILE in `ls -R -1 ../src/*/*.{cc,c,h}`; do astyle --options=astylerc $FILE; done

_cppcheck:
    cppcheck --enable=all ../src/*.cc ../src/*.c ../src/*.h

