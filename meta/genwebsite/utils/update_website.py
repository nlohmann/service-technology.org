#!/usr/bin/env python3

import sys
import json
import markdown
import utils.stella_out

replDict = dict()
j = dict()

# FUNCTIONS

def condInsert(key,f,default=''):
    global j
    global replDict    
    if key in j:
        setFlag(key)
        f(key)
    else:
        replDict[key] = default
        remFlag(key)


def getCommitString(nrOfCommits):
    if nrOfCommits == 1:
        return str(nrOfCommits) + " commit"
    else:
        return str(nrOfCommits) + " commits"

def getHTML(md):
    return markdown.markdown(md)

def simpleInsert(key):
    global replDict
    global j
    if key not in j:
      utils.stella_out.error("Required key >" + key + "< not found.")
    replDict[key] = j[key]

def htmlInsert(key):
    global replDict
    global j
    replDict[key] = getHTML(j[key])

def setFlag(key):
    global replDict
    replDict[key+ 'GIVEN'] = 'normal'

def remFlag(key):
    global replDict
    replDict[key + 'GIVEN'] = 'none'

def buildReqList(key): 
    result = ''
    for req in j[key]:
        curReq = req
        if req in r:
            curReq = '<a href="' + r[req]['url'] + '" title="' + r[req]['name'] + '">' + r[req]['name'] + '</a> - ' + r[req]['desc']
        result += listThis(curReq)    
    return result

def insertToolList(key):
    if key in j:
        replDict[key] = buildReqList(key)
        setFlag(key)
    else:
        replDict['key'] = 'none'
        remFlag(key)

def insertStringEnum(vName): 
    global j
    global replDict
    if (len(j[vName])) == 1:
        replDict[vName] = j[vName][0]

    if (len(j[vName])) == 2:
        replDict[vName] = j[vName][0] + ' and ' + j[vName][1]

    if (len(j[vName])) > 2:
        replDict[vName]  = ''
        for i in range(0, len(j[vName])-1):
            replDict[vName]  = replDict[vName] + j[vName][i] + ", "
        replDict[vName]  = replDict[vName] + 'and ' + j[vName][len(j[vName])-1]

def insertStringList(vName):
    global j
    global replDict
    replDict[vName]  = ''
    if vName in j:
        for thank in j[vName]:
            replDict[vName] = replDict[vName] + listThis(thank)
        setFlag(vName)
    else:
        remFlag(vName)

def insertCommitList(vName, prefix):
    global j
    global p
    global replDict
    contribDict = dict()

    for cont in j[vName]:
        curuser = cont['user']
        if cont['user'] in p['refs']:
            curuser = p['refs'][curuser]

        if curuser in contribDict: 
           contribDict[curuser] = contribDict[curuser] + cont[vName]
        else:
           contribDict[curuser] = cont[vName]

    contribList = sorted(contribDict, key = contribDict.get, reverse=True)

    replDict[prefix + 's'] = ''
    replDict[prefix + 'images'] = '' 
    for cont in contribList:
        actname = cont 
        if cont in p['data']:
            actname = p['data'][cont]['name']
        if cont in p['people']:
            replDict[prefix + 'images'] = replDict[prefix + 'images'] + getImageTag('../people/g/' + cont + '.jpg', actname, actname, 'class="portrait" width="75"')
        else:
                replDict[prefix + 'images'] = replDict[prefix + 'images'] + getImageTag('../people/g/dummy.jpg', actname, actname, 'class="portrait" width="75"')
        replDict[prefix + 's'] = replDict[prefix + 's'] + listThis( actname + " (" + getCommitString(contribDict[cont]) + ")")


def getImageTag(filename, alt, title, additional = ''):
    return '<img src="' + filename + '" alt="' + alt + '" title="' + title + '" ' + additional + '>'

def listThis(someString):
    return '<li>' + someString + '</li>' + "\n"

    
def mineAll(jsonfile, genericfile, peoplefile, reqfile, varfile):
  
  global replDict
  global j
  global p
  global r
  
  j1 = json.loads(open(jsonfile, 'r').read())
  j2 = json.loads(open(genericfile, 'r').read())
  j = dict(list(j1.items()) + list(j2.items()))
  p = json.loads(open(peoplefile, 'r').read())
  r = json.loads(open(reqfile, 'r').read())
  variables = json.loads(open(varfile, 'r').read())

  # BUILDING THE REPLACEMENT DICTIONARY

  # GO THROUGH ALL THE SINGLE VARIABLES
  
  for v in variables["singles"]:
    # GET THE TYPE OF THE VARIABLES
    vName = v["name"]
    vType = ""
    if "type" in v:
      vType = v["type"]
    else:
      vType = "required"
      
    print(vType)

    # IF IT IS OF TYPE RESULT, SKIP THIS VARIABLE
    if vType == "result":
      continue
    
    # GET THE FORMAT OF THE VARIABLE AND SET THE INSERT FUNCTION
    
    insertF = simpleInsert
    if "format" in v:
      if v["format"] == "markdown":
        insertF = htmlInsert
   
    
    # IF IT IS OF TYPE COND, SET THE VALUE ACCORDINGLY
    if vType == "cond":
      defaultString = ""
      if "default-ref" in v:
        defaultString = replDict[v["default-ref"]]
      if "default-val" in v:
        defaultString = v["default-val"]
      if "default-complex" in v:
        compl = v["default-complex"]
        for x in compl:
          if "ref" in "x":
            defaultString += replDict[x["ref"]]
          if "val" in "x":
            defaultString += x["val"]
      condInsert(vName, insertF, defaultString)
      continue
      
    # IT IS NEITHER OF TYPE COND NOR RESULT, SO IT'S EITHER AUTO OR REQUIRED. JUST INSERT IT ACCORDINGLY
    print(vName)
    insertF(vName)
  
  # RETRIEVE THE MAINTAINER STUFF
  
  replDict['maintainerusername'] = ''
  replDict['maintainermail'] = ''
  replDict['maintainerurl'] = ''
  for username in p['data']:
    if p['data'][username]['name'] == j['maintainer']:
      replDict['maintainerusername'] = username
      replDict['maintainermail'] = p['data'][username]['url']
      replDict['maintainerurl'] = p['data'][username]['url']

  # NOW THE MULTI VARIABLES
  
  for v in variables["multis"]:
    vName = v["name"]
    
    # GET THE TYPE OF THE VARIABLES
    vType = ""
    if "type" in v:
      vType = v["type"]
      
    # IF IT IS OF TYPE RESULT, SKIP THIS VARIABLE
    if vType == "result":
      continue    

    vFormat = v["format"]
      
    # IF IT IS A STRING ENUMERATION, MAKE AN COMMA-SEPARATED LIST AND INSERT
    if vFormat == "string-enum":
      insertStringEnum(vName)

    # IF IT IS A STRING LIST, INSERT IT AS HTML LIST
    if vFormat == "string-list":
      insertStringList(vName)
      
    # IF IT IS A COMMIT LIST, COLLECT ALL INFORMATION AND INSERT THE HTML LIST
    if vFormat == "commit-list":
      insertCommitList(vName, v["prefix"])
    
    # IF IT IS A TOOL LIST, COLLECT ALL INFORMATION AND INSERT THE HTML LIST
    if vFormat == "tool-list":
      insertToolList(vName)
 


  
def writeFile(template, target):  
  # READ AND CLOSE TEMPLATE FILE

  global replDict
  
  tfile = open(template, 'r')
  t = tfile.read()
  tfile.close()

  # REPLACE ALL OCCURRENCES IN THE TARGET FILE 

  f = open(target, 'w')

  for someKey in replDict: 
      t = t.replace('@' + someKey.upper() + '@', replDict[someKey])    

  f.write(t)
  f.close()
  




