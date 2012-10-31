#!/usr/bin/env python3

import sys
import json
import os

# Functions

# Do the grep-work

def grepExpr(expr,path):
  f=os.popen("DIR='" + path + "..' && grep -Rh \"" + expr + "\" ${DIR}/m4/* ${DIR}/*.am ${DIR}/*.ac | sort | uniq")
  return f.read() 

def grepEmAll(path):
  expressions =["AM_MISSING_PROG(", "AC_PATH_PROG(", "AC_PROG_CC", "AC_PROG_CXX", "AC_PROG_LEX", "AC_PROG_YACC", "AC_PROG_LIBTOOL" ]
  result = ""
  for expr in expressions:
    result += grepExpr(expr, path)
  return result

def getOfficialVersion(toolname, path):
  f=os.popen("curl -s http://download.gna.org/service-tech/" + toolname + "/ | grep -oh \"" + toolname + "-[0-9.]*\" | sort | uniq | sed \"s/" + toolname + "-//g\" | sed 's/.$//g' | tail -n1")       
  return f.read().strip()

def getFromConfigStatus(variable, path):
  f=os.popen("echo @" + variable + "@ | " + path + "../config.status --file=-")
  return f.read().strip()


def getLastVersion(path):
  return getFromConfigStatus("PACKAGE_VERSION", path)

def getURL(path):
  return getFromConfigStatus("PACKAGE_URL", path)

def getEMail(path):
  return getFromConfigStatus("PACKAGE_BUGREPORT", path)
  

def getCommitters(path): 
  f=os.popen("svn log " + path + "../ --quiet | grep '^r' | awk '{print $3}' | sort | uniq -c | sort -r | awk ' { print \"{ \\\"user\\\": \\\"\" $2 \"\\\", \\\"commits\\\": \" $1 \" }\"  } ' | tr '\n' ',' | sed 's/.$//'")
  return "{ \"commits\" : [" + f.read().strip() + "]}"


def buildReqDict(path):
  l = grepEmAll(path)
  
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



def generate(toolname, path):

  result = { "url" : getURL(path),  "email" : getEMail(path) , "lastVersion" : getLastVersion(path) }
  offVersion = getOfficialVersion(toolname,path)
  if offVersion != "":
    result["officialVersion"] = offVersion 
  result.update(buildReqDict(path))
  result.update(json.loads(getCommitters(path)))

  oldVals = dict()

  if os.path.isfile(path + "generic.json"):
    try:
      oldVals = json.loads(open(path + "generic.json", "r").read())
    except ValueError:
      oldVals = {}

  oldVals.update(result)

  open(path + "generic.json","w").write(json.dumps(oldVals))
 



