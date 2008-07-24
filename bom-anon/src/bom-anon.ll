 /* 
  BOM-ANONYMIZER was written by Niels Lohmann <niels.lohmann@service-technology.org>.

  It is licensed under the Creative Commons Attribution 2.0 Germany License
  (http://creativecommons.org/licenses/by/2.0/de/).
 */

%{
#include <cstdio>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>

#include "config.h"

std::string anonymize(std::string &s);
std::string processString(const char *text);
std::string toString(unsigned int q);

unsigned int nextId = 1;
unsigned int currentView;
std::map<std::string, std::string> string_cache;

// a list of standard values that is not anonymized to avoid parse errors (array must end with empty string)
std::string value_whitelist[] = {"Boolean", "Byte", "Date", "DateTime", "Double", "Duration", "Float", "Integer", "Long", "Short", "String", "Time", "Communication Service", "Equipment", "Facility", "General Service", "Machine", "Tool", "Person", "Staff", "acres", "centimeters", "feet", "gallons (UK)", "gallons (US)", "grams", "hectares", "inches", "kilograms", "kilometers", "liters", "meters", "miles", "ounces", "pints (UK)", "pints (US)", "pounds", "units", "yards", "true", "false", ""};
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


%%


// split a string into two parts if "##" is found; then anonymize it
std::string processString(const char *text) {
    // strip the enclosing quotes
    std::string s(text);
    s = s.substr(1,s.length()-2);
    
    // strip newlines at beginning or end of string
    if (s[0] == '\n')
        s = s.substr(1,s.length()-1);
        
    if (s[s.length()-1] == '\n')
        s = s.substr(0,s.length()-1);

    // specially handle strings with "##" separator
    if (s.find("##") == std::string::npos) {
        return anonymize(s);
    } else {
        // split the string s into two parts: s1 before "##", s2 after
        std::string s1 = s.substr(0, s.find_first_of("##"));
        std::string s2 = s.substr(s.find_first_of("##")+2, s.length());
        
        return anonymize(s1)+"##"+anonymize(s2);
    }

    return s;
}


// anonymize a string by replacing it with some numbered string
std::string anonymize(std::string &s) {
    // only if string was not already anonymized, create a new name
    if (string_cache[s] == "") {
        string_cache[s] = "s" + toString(nextId++);
    }
    
    return string_cache[s];
}


// convert an unsigned int to a string 
std::string toString(unsigned int q) {
    std::ostringstream buffer;    
    buffer << std::setw(8) << std::setfill('0') << q;
    
    return buffer.str();
}


int main(int argc, char** argv) {
    // for debugging purposes
    if (argc == 2 && std::string(argv[1]) == "--bug") {
        printf("\n\n");
        printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
        printf("- tool:              %s\n", PACKAGE_NAME);
        printf("- version:           %s\n", PACKAGE_VERSION);
        printf("- compilation date:  %s\n", __DATE__);
        printf("- compiler version:  %s\n", __VERSION__);
        printf("- platform:          %s\n", BUILDSYSTEM);
        printf("\n\n");
        return EXIT_SUCCESS;
    }
    
    // initialize string cache with value whitelist to avoid anonymizing thereof
    for (unsigned int i = 0; value_whitelist[i] != ""; i++) {
        string_cache[value_whitelist[i]] = value_whitelist[i];
    }
    
    int flex_result = yylex();
    
    // reverse the cache for output of the mapping
    std::map<std::string,std::string> reversed_cache;
    for (std::map<std::string,std::string>::iterator it = string_cache.begin();
         it != string_cache.end(); it++) {
        reversed_cache[it->second] = it->first;
    }

    for (unsigned int i = 1; i < nextId; i++) {
        fprintf(stderr, "\"%s\" <- \"%s\"\n",
                ("s" + toString(i)).c_str(),
                reversed_cache["s" + toString(i)].c_str());
    }
    
    string_cache.clear();
    reversed_cache.clear();
    
    return flex_result;
}
