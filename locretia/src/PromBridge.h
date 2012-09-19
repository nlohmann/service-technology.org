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

#pragma once

#include "jni.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

/*!
 \brief Prom bridge

 A class that uses Prom and specifically The PNAlignment plugin
*/
class PromBridge {

    public: /* static functions */

		static JNIEnv* create_vm(std::string & javaPath, JavaVM* & jvm);

		/// invokes the Prom-classes
		static void invoke_class(JNIEnv* env, std::string & pnmlPath, std::string & logPath);

		/// calls Prom with given log and petri net
		static int callProm(std::string & pnmlPath, std::string & logPath, std::string & javaPath);

    public: /* static attributes */

    private: /* static attributes */

    private: /* static functions */

    public: /* member functions */

    private: /* member functions */

    public: /* member attributes */

};
