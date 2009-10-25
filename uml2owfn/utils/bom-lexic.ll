 /* 
  BOM-ANONYMIZER was written by Niels Lohmann <niels.lohmann@service-technology.org>.

  It is licensed under the Creative Commons Attribution 2.0 Germany License
  (http://creativecommons.org/licenses/by/2.0/de/).
 */

%{
#include <cstdio>
#include <string>

extern std::string processString(const char *text);
extern unsigned int currentView;
%}

 /*
    A list of technical attributes that are not anonymized to avoid parse errors.
    The list is prefixed by any non-alphanumerical letter to avoid matching of
    attribute names that have a whitelisted attribute name as suffix, for example
    "fooalpha".
  */
attrib_whitelist [^a-zA-Z0-9]("a"|"alpha"|"atBeginning"|"b"|"beginingOn"|"beta"|"callSynchronously"|"capacity"|"counterIncrement"|"currency"|"dayOfWeek"|"delta"|"duration"|"expMean"|"finalCounter"|"gamma"|"initialCounter"|"isConditionTestedFirst"|"isConsumable"|"isInclusive"|"isInsert"|"isObserveContinuously"|"isOrdered"|"isReadOnly"|"isRemove"|"isSimpleDecision"|"isStatic"|"isUnique"|"k"|"lambda"|"max"|"maximum"|"mean"|"min"|"minimum"|"mode"|"multipleInstancesSatisfyCondition"|"noInstancesSatisfyCondition"|"numberOfTimesToRepeat"|"probability"|"repetitionPeriod"|"schemaVersion"|"standardDeviation"|"startTime"|"time"|"timeRequired"|"timeUnit"|"unit"|"unit"|"valueType"|"weekNumber"|"xi"|"version"|"encoding"|"xmlns:wbim")

name             [:alpha:][:alnum:]*
number           [0-9]+
esc              "&#"{number}";"|"&#x"[0-9a-fA-F]+";"
quote            \"
string           {quote}([^"]|{esc})*{quote}
ns               "wbim:"?
desc_end         "</"{ns}"description>"
anno_end         "</"{ns}"annotationText>"
docu_end         "</"{ns}"documentation>"
literal_end      "</"{ns}"literalValue>"
query_end        "</"{ns}"individualResourceRequirement>"
real             {number}"."{number}
whitespace       [ \t\r\n]*
xsdduration      "P"({number}"Y")?({number}"M")?({number}"D")?("T"({number}"H")?({number}"M")?({number}"S")?)?
xsddatetime      [0-9]{4}"-"[0-9]{2}"-"[0-9]{2}"T"[0-9]{2}":"[0-9]{2}":"[0-9]{2}("."{number})?"Z"


attributes       ([[:alnum:]]+"="{string}{whitespace})*

%option noyywrap
%option nounput

%s COMMENT
%s QUERY
%s PROBABILITY


%%

 /* literal values inside <wbim:probability> are not anonymized */
 // <PROBABILITY>"<"{ns}"probability>" { BEGIN(currentView); printf(yytext);  }
<PROBABILITY>"<"{ns}"literalValue>"{whitespace}{real}{whitespace}"</"{ns}"literalValue>" { printf(yytext); }
<PROBABILITY>.                     { printf(yytext); }
"<"{ns}"probability>"              { currentView = YY_START; BEGIN(PROBABILITY); printf(yytext); }

 /* real numbers in any element are replaced by 0.0 */
<INITIAL>{real} { printf("0.0"); }

 /* keep xsd:duration and xsd:dateTime */
"<"{ns}"literalValue>"{xsdduration}"</"{ns}"literalValue>" { printf(yytext); }
"<"{ns}"startTime>"{xsddatetime}"</"{ns}"startTime>"       { printf(yytext); }


 /* replace comments, <wbim:individualResourceRequirement> elements */
"<"{ns}"individualResourceRequirement "{attributes}"/>" { printf("<wbim:individualResourceRequirement individualResource=\"Person\" name=\"Individual requirement:1\" timeRequired=\"P0Y0M0DT0H0M0S\"/>"); }
"<"{ns}"individualResourceRequirement "{attributes}">"  { currentView = YY_START; BEGIN(QUERY); }
<QUERY>.                              { /* skip */ }
<QUERY>{query_end}                    { BEGIN(currentView); printf("<wbim:individualResourceRequirement individualResource=\"Person\" name=\"Individual requirement:1\" timeRequired=\"P0Y0M0DT0H0M0S\"/>"); }


 /* skip comments, <documentation>, <description>, and <annotationText>. Replace literalValues by "0.0" */
"<!--"|"<"{ns}"annotationText>"|"<"{ns}"description>"|"<"{ns}"documentation>"|"<"{ns}"literalValue>" { currentView = YY_START; BEGIN(COMMENT); }
<COMMENT>"-->"         { BEGIN(currentView); printf("<!-- ... -->");}
<COMMENT>{docu_end}    { BEGIN(currentView); printf("<wbim:documentation> ... </wbim:documentation>"); }
<COMMENT>{desc_end}    { BEGIN(currentView); printf("<wbim:description> ... </wbim:description>"); }
<COMMENT>{anno_end}    { BEGIN(currentView); printf("<wbim:annotationText> ... </wbim:annotationText>"); }
<COMMENT>{literal_end} { BEGIN(currentView); printf("<wbim:literalValue>0.0</wbim:literalValue>"); }
<COMMENT>.             { /* skip */ }


 /* attributes on the whitelist are not anonymized */
{attrib_whitelist}"="{string}  { printf(yytext); }

 /* any string checked if it has to be split and/or anonymized */
{string}  { printf("\"%s\"", processString(yytext).c_str()); }
