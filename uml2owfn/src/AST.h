/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

class ASTElement;
class ASTLiteral;
class ASTNode;

#ifndef AST_H
#define AST_H


/******************************************************************************
 * Headers
 *****************************************************************************/

#include <list>
#include <string>
#include <map>

#include "AST-public.h"

#include "config.h"
#include "internal-representation.h"

using std::string;
using std::map;
using std::list;

/******************************************************************************
 * Classes
 *****************************************************************************/

class ASTElement
{
    protected:
        // pointer to the parent node
        ASTNode * parent;

    public:
        /// standard constructor
        ASTElement();

        // returns the pointer to the parent-node
        ASTNode* getParent();

        // generates a debug output on the console for this ASTElement
        virtual void debugOutput(string delay) {}

        // First dive in the XML tree collects all global tasks, services
        // and processes, so that callTo-tasks can instantiate them
        virtual void prepareInstances(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services) {}

        // Completly translates the information from the XML tree into the
        // internal representation of the BOM model
        virtual void finishInternal(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services, list<Process*>& realProcesses) {}

        // generic destructor
        virtual ~ASTElement() {}
};

class ASTLiteral: public ASTElement
{
    private:
        // A string representing the literal itself
        string content;

    public:
        // constructor, needing the parent node and the content
        ASTLiteral(string literalContent, ASTNode* parentNode);

        // generic destructor
        ~ASTLiteral();

        // generates a debug output on the console for this ASTElement
        void debugOutput(string delay);

        // First dive in the XML tree collects all global tasks, services
        // and processes, so that callTo-tasks can instantiate them
        void prepareInstances(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services);

        // Completly translates the information from the XML tree into the
        // internal representation of the BOM model
        void finishInternal(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services, list<Process*>& realProcesses);

};

class ASTNode: public ASTElement
{
    private:
        // a list containing pointers to all children of this AST node
        list<ASTElement*> children;

        // The tag of the AST-Node
        string tag;

        // a map containing all of the tags attributes
        map<string, string> attributes;

    public:
        // constructor, needing the parent node and the content
        ASTNode(string nodeTag, ASTNode* parentNode);

        // adds an Attribute to the attribute map
        void addAttribute(string attribute, string value);

        // adds an Attribute to the attribute map
        void addChild(ASTElement* child);

        // destructor
        ~ASTNode();

        // generates a debug output on the console for this ASTElement
        void debugOutput(string delay);

        // First dive in the XML tree collects all global tasks, services
        // and processes, so that callTo-tasks can instantiate them
        void prepareInstances(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services);

        // Completly translates the information from the XML tree into the
        // internal representation of the BOM model
        void finishInternal(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services, list<Process*>& finalProcesses);
};

#endif
