/*
parse GGO
*/


start
  = ITEM_LIST

ITEM_LIST = items:ITEM+ SP* {
var cleanList = [];
itemsLength = items.length;
for(var i=0;i<itemsLength;++i) {
var c=items[i];
if(typeof c != 'undefined') cleanList.push(c);
}
return cleanList;
}

ITEM "ITEM" = SP* i:(PURPOSE / IGNORE / SECTION / OPTION / COMMENT / ARGS) { return i; }

ARGS = "args" SP+ "\"" [\"]* s:[^\"]* "\"" { return { top:true, unamed_opts_file : (s.join('').indexOf('--unamed-opts=FILE')>-1) }; }

PURPOSE = "purpose" (SP* "\"" s:([^\"]*)  "\"") { return {purpose: s.join("")}; }

IGNORE = IGNORE_WORD ([ ]* "\"" [^\"]*  "\"")? { return; }

//lowest prio
COMMENT = SP* "#" [^\n]* {return;}


IGNORE_WORD = "description"

SECTION = "section" SP+ s:STRING SP+
         secdesc:("sectiondesc" SP* "=" SP* ds:STRING {return ds;})?
{
  return { section: s, sectiondesc: secdesc };
}

OPTION = "option" SP+
         long:IDENTIFIER SP+
         short:IDENTIFIER SP+
         desc:STRING SP+
         properties:OPTION_PROPERTIES?
{ 
var obj = { long:long, short:short, desc:desc};
var pLen = properties.length;
for(var i=0; i<pLen; ++i) {
    var c = properties[i];
    if(c)
        obj[c[0]]=c[1];
}
return obj;
}


ARGTYPE = a:("int" / "float" / "short" / "string" / "double" / "longdouble" / "longlong" / "enum") {
return ['argtype',a];}

OPTION_PROPERTIES =
op:OPTION_PROPERTY SP+ oplist:OPTION_PROPERTIES
{ return [op].concat(oplist);} / op:OPTION_PROPERTY { return [op]; }

OPTION_PROPERTY = p:(SINGLE_PROPERTY / KEY_VAL_PROPERTY / VALUES_PROPERTY / FLAG / ARGTYPE / LIVE_COMMENT) { return p; }

LIVE_COMMENT = SP* "#" l:LIVE_COMMENT_OPT  { if(l) return l; else return; }
LIVE_COMMENT_OPT = "live"i SP+ kv:KEY_VAL_PROPERTY { return kv; }
                 / "live"i SP+ id:IDENTIFIER { return [id,true]; }
                 / [^\n]* { return false;}

SINGLE_PROPERTY = s:("required" / "argoptional" / "multiple" / "hidden" / "optional") { return [s,true]; }

FLAG = f:"flag" SP+ o:("on"/"off") { return [f,o]; }

KEY_VAL_PROPERTY = key:("default" / "details" / "dependon" / "typestr") SP* "=" SP* val:STRING
{return [key,val];}

VALUES_PROPERTY = "values" SP* "=" SP* list:(STRING "," SP*)* last:STRING SP+ "enum"?
{
var vals = [];
for(var i=0,c=null;c=list[i];++i) {
vals.push(c[0]);
}
vals.push(last);
return ['values',vals];
}

/// LEXER

SP = [ \n] { return ''; }

IDENTIFIER = "\"" id:[_a-zA-z0-9\-]+ "\"" { return id.join('');; } /
id:[_a-zA-z0-9\-]+ { return id.join('');; }

STRING = "\"" s:[^\"]+ "\"" {return s.join('');}

