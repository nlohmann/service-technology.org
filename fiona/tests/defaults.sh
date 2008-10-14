# Allows test scrips to be directly called from the command line. Set all unset
# environment variables that are normally set by the Makefile.

if [ "$testdir" == "" ]; then
    testdir=.
fi

if [ "$builddir" == "" ]; then
    builddir=.
fi

if [ "$MKDIR_P" == "" ]; then
    MKDIR_P="mkdir -p"
fi

evaluate="evaluateExitCode.sh"