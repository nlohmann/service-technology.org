 /*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <iostream>
#include <sstream>
#include "PromBridge.h"

using namespace std;

/******************
 * STATIC MEMBERS *
 ******************/


/******************
 * STATIC METHODS *
 ******************/

JNIEnv* PromBridge::create_vm(std::string & javaPath, JavaVM* & jvm) {

	JNIEnv* env;
	JavaVMInitArgs args;
	JavaVMOption options[1];

	/* There is a new JNI_VERSION_1_4, but it doesn't add anything for the purposes of our example. */
	args.version = JNI_VERSION_1_6;

	// default Argumente holen:
	JNI_GetDefaultJavaVMInitArgs((void *)&args);

	stringstream tmp (stringstream::in | stringstream::out);
	tmp << "-Djava.class.path=" << javaPath.c_str();

	string temp = tmp.str();

	args.nOptions = 1;
	options[0].optionString = const_cast<char*>(temp.c_str());
	args.options = options;
	args.ignoreUnrecognized = JNI_FALSE;

	// zur Fehlererkennung:
	jint value;

	// JavaVM erzeugen:
	value = JNI_CreateJavaVM(&jvm, (void **)&env, &args);
	if(value < 0)
	{
		switch(value)
		{
		case JNI_ERR:
			cout << value << " : JNI_ERR";
			break;
		case JNI_EDETACHED:
			cout << value << " : JNI_EDETACHED";
			break;
		case JNI_EVERSION:
			cout << value << " : JNI_EVERSION";
			break;
			//		case JNI_ENOMEM:
			//			cout << "%i : JNI_ENOMEM", value);
			//			break;
			//		case JNI_EEXIST:
			//			cout << "%i : JNI_EEXIST", value);
			//			break;
			//		case JNI_EINVAL:
			//			cout << "%i : JNI_EINVAL", value);
			//			break;
		}
	}
	else
		cout << "CreateJavaVM success!!" << endl;

	return env;
}

void PromBridge::invoke_class(JNIEnv* env, std::string & pnmlPath, std::string & logPath) {
	jclass helloWorldClass;
	jmethodID mainMethod;
	jobjectArray applicationArgs;
	jstring applicationArg0;
	jstring applicationArg1;

	//cout << "1" << endl;

	helloWorldClass = env->FindClass("prom/consoleReplayer");

	if (helloWorldClass) {
		cout << "FindClass success!!" << endl;
	} else {
		cout << "FindClass failed :(" << endl;
		env->ExceptionDescribe();
	}

	//cout << "2" << endl;
	mainMethod = env->GetStaticMethodID(helloWorldClass, "main", "([Ljava/lang/String;)V");

	//cout << "3" << endl;
	applicationArgs = env->NewObjectArray(2, env->FindClass("java/lang/String"), NULL);
	applicationArg0 = env->NewStringUTF(pnmlPath.c_str());
	applicationArg1 = env->NewStringUTF(logPath.c_str());

	//cout << "4" << endl;
	env->SetObjectArrayElement(applicationArgs, 0, applicationArg0);
	env->SetObjectArrayElement(applicationArgs, 1, applicationArg1);

	//cout << "5" << endl;
	env->CallStaticVoidMethod(helloWorldClass, mainMethod, applicationArgs);

	//cout << "6" << endl;
}

int PromBridge::callProm(std::string & pnmlPath, std::string & logPath, std::string & javaPath) {

	JavaVM* jvm;

	//cout << "create VM" << endl;
	JNIEnv* env = create_vm(javaPath, jvm);
	//cout << "invoke class" << endl;
	invoke_class(env, pnmlPath, logPath);

	jvm->DestroyJavaVM();

	return 0;
}
