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
