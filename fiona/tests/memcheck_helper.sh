#!/bin/bash

valgrind_opts="--leak-check=full --show-reachable=yes --freelist-vol=50000000"

do_memcheck()
{
    if [ ! -x "`which valgrind 2>/dev/null`" ]; then
        echo valgrind is not in path. Unable to do memcheck.
        return 1
    fi
    cmd=$1
    memchecklog=$2
    echo memchecking $cmd
    valgrind_cmd="valgrind $valgrind_opts $cmd"
    echo $valgrind_cmd >"$memchecklog"
    $valgrind_cmd 2>>"$memchecklog" >/dev/null
    memchecklog_check_for_errors "$memchecklog"
    if [ $? -ne 0 ]; then
        echo "Memory ERRORS detected. Consult $memchecklog."
        return 1
    fi
    memchecklog_check_for_leaks "$memchecklog"
    if [ $? -ne 0 ]; then
        echo "Memory leaks detected. Consult $memchecklog."
        return 1
    fi

    return 0
}

memchecklog_check_for_errors()
{
    memchecklog=$1
    grep "ERROR SUMMARY: 0 errors" "$memchecklog" >/dev/null
    return $?
}

memchecklog_check_for_leaks()
{
    memchecklog=$1
    grep "definitely lost: 0 bytes" "$memchecklog" >/dev/null
    if [ $? -ne 0 ]; then
        return 1
    fi
    grep "possibly lost: 0 bytes" "$memchecklog" >/dev/null
    if [ $? -ne 0 ]; then
        return 1
    fi
    grep "still reachable: 0" "$memchecklog" >/dev/null
    if [ $? -ne 0 ]; then
        return 1
    fi

    return 0
}
