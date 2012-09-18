#!/usr/bin/env python3

import sys
import json
import os

# Functions

# Do the grep-work

def grepExpr(expr):
  f=os.popen("DIR='..' && grep -Rh \"" + expr + "\" ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq")
  return f.read() 

def grepEmAll():
  expressions =["AM_MISSING_PROG(", "AC_PATH_PROG(", "AC_PROG_CC", "AC_PROG_CXX", "AC_PROG_LEX", "AC_PROG_YACC", "AC_PROG_LIBTOOL" ]
  result = ""
  for expr in expressions:
    result += grepExpr(expr)
  return result

def getOfficialVersion(toolname):
  f=os.popen("curl -s http://download.gna.org/service-tech/" + toolname + "/ | grep -oh \"" + toolname + "-[0-9.]*\" | sort | uniq | sed \"s/" + toolname + "-//g\" | sed 's/.$//g' | tail -n1")       
  return f.read().strip()

def getFromConfigStatus(variable):
  f=os.popen("echo @" + variable + "@ | ../config.status --file=-")
  return f.read().strip()


def getLastVersion():
  return getFromConfigStatus("PACKAGE_VERSION")

def getURL():
  return getFromConfigStatus("PACKAGE_URL")

def getEMail():
  return getFromConfigStatus("PACKAGE_BUGREPORT")
  

def getCommitters(): 
  f=os.popen("svn log .. --quiet | grep '^r' | awk '{print $3}' | sort | uniq -c | sort -r | awk ' { print \"{ \\\"user\\\": \\\"\" $2 \"\\\", \\\"commits\\\": \" $1 \" }\"  } ' | tr '\n' ',' | sed 's/.$//'")
  return "{ \"commits\" : [" + f.read().strip() + "]}"


def buildReqDict():
  l = grepEmAll()
  
  d_compile = { "AC_PROG_CC" : "gcc", "AC_PROG_CXX" : "g++" , "AC_PROG_LIBTOOL" : "libtool"}
  d_tests = { "autom4te" : "autotest" , "lcov" : "lcov" , "valgrind" : "valgrind" }
  d_manipulation = { "AC_PROG_YACC" : "bison" , "AC_PROG_LEX" : "flex" , "gengetopt" : "gengetopt", "kc++" : "kc++" }

  req_compile = []
  for key in d_compile:
    if l.count(key) > 0:
      req_compile.append(d_compile[key])

  req_tests = []
  for key in d_tests:
    if l.count(key) > 0:
      req_tests.append(d_tests[key])
  
  req_manipulation = []
  for key in d_manipulation:
    if l.count(key) > 0:
      req_manipulation.append(d_manipulation[key])

  return { "req_compile" : req_compile , "req_tests" : req_tests, "req_edit" : req_manipulation }







toolname = json.loads(open(sys.argv[1]).read())["shortname"]

result = { "url" : getURL(),  "email" : getEMail() , "officialVersion" : getOfficialVersion(toolname) , "lastVersion" : getLastVersion() ,  "commits" : json.loads(getCommitters()) }
result.update(buildReqDict())


oldVals = dict()

if os.path.isfile("generic.json"):
  try:
    oldVals = json.loads(open("generic.json", "r").read())
  except ValueError:
    oldVals = {}

oldVals.update(result)

open("generic.json","w").write(json.dumps(oldVals))
 



