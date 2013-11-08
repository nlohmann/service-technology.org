.PHONY : _cppcheck _style

_style:
	for FILE in `ls -R -1 ../src/*/*.{cc,c,h} ../src/*/*/*.{cc,c,h}`; do astyle --options=astylerc $$FILE; done

_cppcheck:
	cppcheck --enable=all --inconclusive --xml ../src --force 2> cppcheck.xml
