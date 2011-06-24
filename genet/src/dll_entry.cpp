#include "config.h"

#ifdef HAVE_JNI_H
// #include <jni.h>
#include "cpp_interface.h"
#endif
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "Output.h"

extern int genet_init(int, char**);

#define FUNC std::cerr << " ... in function " << __PRETTY_FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl;

extern "C" {
    char* genetCall (char*, int, char**);
}

char * genetCall(char * input, int argc, char ** argv)
{
    std::string inName;
    std::string outName;

    {
        Output genetInputData;
        inName = genetInputData.name();
        outName = inName + ".pn";
        Output genetOutputData(outName,"output file for Genet");

        genetInputData.stream() << input << std::flush;
        genetOutputData.stream() << std::endl << std::flush;
    }

    char ** genetArgs = (char **)malloc((argc + 4) * sizeof(char*));
    genetArgs[0] = strdup("genetDLL");
    for(unsigned int argumentIndex = 0; argumentIndex < argc; ++argumentIndex)
    {
        genetArgs[argumentIndex + 1] = strdup(argv[argumentIndex]);
    }
    genetArgs[argc + 1] = strdup(inName.c_str());
    genetArgs[argc + 2] = strdup("-o");
    genetArgs[argc + 3] = strdup(outName.c_str());

    genet_init(argc + 4, genetArgs);

    // do something

    std::ifstream resultFile(outName.c_str(), std::ios_base::in);
    std::string resultData;
    while (not resultFile.eof())
    {
        resultData += resultFile.get();
    }
    resultFile.close();
    resultData = resultData.substr(0, resultData.length()-1);

    remove(inName.c_str());
    remove(outName.c_str());

    return strdup(resultData.c_str());
}

#ifdef HAVE_JNI_H
JNIEXPORT jstring JNICALL Java_edu_upc_lsi_genet_Genet_genetCall
(JNIEnv * env, jclass, jstring input, jint bound)
{
    std::string test(env->GetStringUTFChars(input, 0));
    std::stringstream number;
    number << bound;

    char * argv[] = { strdup("-k"),
                      strdup(number.str().c_str())
                    };

    return env->NewStringUTF(genetCall(strdup(test.c_str()), 2, argv));
}
#endif


