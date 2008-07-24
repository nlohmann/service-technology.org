#ifndef UNFOLD_H
#define UNFOLD_H

#include"net.H"
#include"dimensions.H"

typedef enum
{
        arr,rec,num,boo,enu //,set, lst
} UTypeClass;

class UValue;

class UType
{
        public:
        UTypeClass tag;
        unsigned long int size;
        virtual bool iscompatible(UType *) = 0;
	virtual UValue * make() = 0;
};

class UNumType: public UType
{
        public:
        bool iscompatible(UType*); 
        int lower;
        int upper;
        UNumType(int,int);
	UValue * make();
};  

class UBooType: public UType
{
        public:
        UBooType();
        bool iscompatible(UType*);     
	UValue * make();
};  

class EnSymbol;

class UEnList
{
	public:
	EnSymbol * sy;
	int v;
	UEnList * next;
};

class UEnuType: public UType
{
        public:
	EnSymbol ** tags;
        bool iscompatible(UType*);  
	UEnuType(UEnList *);
	UValue * make();
};  

class UArrType: public UType
{
        public:
	UType * index;
	UType * component;
        bool iscompatible(UType*); 
        UArrType(UType *, UType *);
	UValue * make();
};  

//class ULstType: public UType
//{
        //public:
	//UType * component;
	//unsigned int maxlength;
        //bool iscompatible(UType*); 
        //ULstType(UType *, unsigned int);
	//UValue * make();
//};  

//class USetType: public UType
//{
        //public:
	//UType * component;
        //bool iscompatible(UType*);
        //USetType(UType *);
	//UValue * make();
//};  

class RcSymbol;

class URcList
{
	public:
	RcSymbol * sy;
	UType * ty;
	URcList * next;
};

class URecType: public UType
{
        public:
	int card;
	RcSymbol ** tags;
	UType ** component;
        bool iscompatible(UType*);  
	URecType(URcList *);
	UValue * make();
};  

class UValue
{
	public:
	UType * type;
	//unsigned long int numeric;
	virtual UValue * copy() = 0;
	virtual void assign(UValue *) = 0;
	virtual bool isfirst() = 0;
	virtual void print(ostream &) = 0;
	virtual void operator ++ (int) = 0;
	virtual bool iseqqual(UValue *) = 0;
	virtual bool islesseqqual(UValue *) = 0;
	virtual UValue * addop(UValue *) = 0;
	virtual UValue * subop(UValue *) = 0;
	virtual UValue * mulop(UValue *) = 0;
	virtual UValue * divop(UValue *) = 0;
	virtual UValue * modop(UValue *) = 0;
	virtual UValue * negop() = 0;
	virtual char * text() = 0;
	UValue(void){};
	virtual ~UValue(void){}
};

inline ostream& operator << (ostream & os, UValue * val)
{
	val->print(os);
}
	
class UNumValue: public UValue
{
	public:
	int v;
	UValue * copy();
	void operator ++ (int);
	virtual bool isfirst();
	void print(ostream &);
	void assign(UValue *);
	virtual bool iseqqual(UValue *);
	virtual bool islesseqqual(UValue *);
	UValue * addop(UValue *);
	UValue * subop(UValue *);
	UValue * mulop(UValue *);
	UValue * divop(UValue *);
	UValue * modop(UValue *);
	UValue * negop();
	char * text();
};	

class UBooValue: public UValue
{
	public:
	bool v;
	UValue * copy();
	void operator ++ (int);
	void print(ostream &);
	void assign(UValue *);
	virtual bool iseqqual(UValue *);
	virtual bool isfirst();
	virtual bool islesseqqual(UValue *);
	UValue * addop(UValue *);
	UValue * subop(UValue *);
	UValue * mulop(UValue *);
	UValue * divop(UValue *);
	UValue * modop(UValue *);
	UValue * negop();
	char * text();
};	

class UEnuValue: public UValue
{
	public:
	int v;
	UValue * copy();
	void operator ++ (int);
	void print(ostream &);
	void assign(UValue *);
	virtual bool iseqqual(UValue *);
	virtual bool isfirst();
	virtual bool islesseqqual(UValue *);
	UValue * addop(UValue *){}
	UValue * subop(UValue *){}
	UValue * mulop(UValue *){}
	UValue * divop(UValue *){}
	UValue * modop(UValue *){}
	UValue * negop(){}
	char * text();
};	

//class ULstValue : public UValue
//{
	//public:
	//unsigned int length;
	//UValue ** content;
	//UValue * copy();
	//void operator ++ (int);
	//void print(ostream &);
	//void assign(UValue *);
	//virtual bool iseqqual(UValue *);
	//virtual bool islesseqqual(UValue *);
	//UValue * addop(UValue *);
//};

class UArrValue : public UValue
{
	public:
	UValue ** content;
	UValue * copy();
	void operator ++ (int);
	void print(ostream &);
	void assign(UValue *);
	virtual bool iseqqual(UValue *);
	virtual bool isfirst();
	virtual bool islesseqqual(UValue *);
	UValue * addop(UValue *);
	UValue * subop(UValue *);
	UValue * mulop(UValue *);
	UValue * divop(UValue *);
	UValue * modop(UValue *);
	UValue * negop();
	char * text();
};

//class USetValue : public UValue
//{
	//public:
	//bool * content;
	//UValue * copy();
	//void operator ++ (int);
	//void print(ostream &);
	//void assign(UValue *);
	//virtual bool iseqqual(UValue *);
	//virtual bool islesseqqual(UValue *);
	//UValue * addop(UValue *);
//};

class URecValue : public UValue
{
	public:
	UValue ** content;
	UValue * copy();
	void operator ++ (int);
	void print(ostream &);
	virtual bool isfirst();
	void assign(UValue *);
	virtual bool iseqqual(UValue *);
	virtual bool islesseqqual(UValue *);
	UValue * addop(UValue *);
	UValue * subop(UValue *);
	UValue * mulop(UValue *);
	UValue * divop(UValue *);
	UValue * modop(UValue *);
	UValue * negop();
	char * text();
};

class UExpression
{
	public:
	UType * type;
	virtual UValue * evaluate() = 0;
};

//class DerefList;

class ULVal
{
	public:
	UType * type;
	virtual UValue * dereference() = 0;
};

class UVar;

class UVarLVal : public ULVal
{
	public:
	UVar * var;
	UValue * dereference();
};

class URecordLVal : public ULVal
{
	public:
	ULVal * parent;
	unsigned int  offset;
	UValue * dereference();
};

class UArrayLVal : public ULVal
{
	public:
	ULVal * parent;
	UExpression * idx;
	UType * indextype;
	UValue * dereference();
};

//class DerefList 
//{
	//public:
	//UValue * val;
	//unsigned long int oldnumeric;
	//DerefList * next;
	//virtual void correct() = 0;
//};

class UStatement
{
	public:
	virtual void execute() = 0;
};

class UWhileStatement : public UStatement
{
	public:
	UExpression * cond;
	UStatement * body;
	void execute();
};

class UAssignStatement : public UStatement
{
	public:
	UExpression * right;
	ULVal * left;
	void execute();
};

class UFunction;

class UReturnStatement : public UStatement
{
	public:
	UExpression * exp;
	UFunction * fct;
	void execute();
};

class UConditionalStatement : public UStatement
{
	public:
	UExpression * cond;
	UStatement * yes;
	UStatement * no;
	void execute();
};	

class UCaseStatement : public UStatement
{
	public:
	int card;
	UExpression * exp;
	UExpression ** cond;
	UStatement ** yes;
	UStatement * def;
	void execute();
};


class UForStatement : public UStatement
{
	public:
	UVar * var;
	UExpression * init;
	UExpression * finit;
	UStatement * body;
	void execute();
};
	
class UExitStatement : public UStatement
{
	public:
	UFunction * fct;
	void execute();
};

class USequenceStatement : public UStatement
{
	public:
	UStatement * first;
	UStatement * second;
	void execute();
};

class UNopStatement: public UStatement
{
	public:
	void execute(){}
};

class UForallStatement : public UStatement
{
	public:
	UVar * var;
	UStatement * body;
	void execute();
};

//class UForinStatement : public UStatement
//{
	//public:
	//UVar * var;
	//UStatement * body;
	//UValue * set;
	//void execute();
//};

class URepeatStatement : public UStatement
{
	public:
	UExpression * cond;
	UStatement * body;
	void execute();
};

class UCallExpression : public UExpression
{
	public:
	UFunction * fct;
	UExpression ** currentpar;
	UValue * evaluate();
};

class UEqualExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UGreatereqqualExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

//class UPrefixExpression : public UExpression
//{
	//public:
	//UExpression * left;
	//UExpression * right;
	//UValue * evaluate();
//};

//class UInExpression : public UExpression
//{
	//public:
	//UExpression * left;
	//UExpression * right;
	//UValue * evaluate();
//};

class USubExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UMulExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UDivExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UModExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UNegExpression : public UExpression
{
	public:
	UExpression * left;
	UValue * evaluate();
};

class UAddExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UGreaterthanExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class ULessthanExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UFalseExpression : public UExpression
{
	public:
	UValue * evaluate();
};

class UTrueExpression : public UExpression
{
	public:
	UValue * evaluate();
};

class UEnuconstantExpression : public UExpression
{
	public:
	unsigned int nu;
	UValue * evaluate();
};

class UIntconstantExpression : public UExpression
{
	public:
	unsigned int nu;
	UValue * evaluate();
};

class ULvalExpression : public UExpression
{
	public:
	ULVal * lval;
	UValue * evaluate();
};

class URecordExpression : public UExpression
{
	public:
	unsigned int card;
	UExpression ** cont;
	UValue * evaluate();
};

class UArrayExpression : public UExpression
{
	public:
	unsigned int card;
	UExpression ** cont;
	UValue * evaluate();
};

class ULesseqqualExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};

class UUneqqualExpression : public UExpression
{
	public:
	UExpression * left;
	UExpression * right;
	UValue * evaluate();
};


class UValueList
{
	public:
		UValue * val;
		UValueList * next;
};

class SymbolTab;

class UResultList
{
	public:
		UValueList * res;
		UResultList * next;
};

class UFunction
{
	public:
	int arity;
	UType * type;
	UValueList * result;
	UResultList * resultstack;
	UVar ** formalpar;
	SymbolTab * localsymb;
	UStatement * body;
	void addresult(UValue *);
	void enter();
	void leave();
};

class UVar
{
	public:
	UType * type;
	UValue * value;
	UValueList * stack;
	void assign(UValue *);
	UValue * get();
	UVar(UType *);
};


bool deep_compatible(UType *,UType *,UTypeClass);
bool deep_compatible(UType *,UTypeClass);

class UTerm
{
	public:
	UType * type;
	virtual UValueList * evaluate() = 0;
};

class UVarTerm: public UTerm
{
	public:
	UVar * v;
	UValueList * evaluate();
};

class UOpTerm: public UTerm
{
	public:
	unsigned int arity;
	UFunction * f;
	UTerm ** sub;
	UValueList * evaluate();
};


class UTermList
{
	public:
	UTerm * t;
	unsigned int mult;
	UTermList * next;
};

class ExitException{}; //Exit-Anweisung wird als Exception realisiert

class VaSymbol;

class fmode
{
	public:
	VaSymbol * v;
	UExpression * t;
	fmode * next;
};

#endif

