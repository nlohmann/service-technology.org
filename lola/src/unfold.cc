#include "net.H"
#include "dimensions.H"
#include "unfold.H"
#include "symboltab.H"
#include <strings.h>
#include <stdio.h>

extern void yyerror(char const *);

UBooType * TheBooType;
UNumType * TheNumType;

UNumType::UNumType(int low,int up)
{
        if(low > up) yyerror("negative range in integer type definition");
        lower = low;
        upper = up;
        size = up - low + 1;
        tag = num;
}

bool UNumType::iscompatible(UType * t)
{
        if(t->tag != num) return false;
        return true;
}    

UValue * UNumType::make()
{
	UNumValue * val;
	val = new UNumValue();
	val -> type = this;
	val -> v = lower;
	//val -> numeric = 0;
	return val;
}

UValue * UBooType::make()
{
	UBooValue * val;
	val = new UBooValue();
	val -> type = this;
	val -> v = false;
	//val -> numeric = 0;
	return val;
}

UBooType::UBooType()
{
        size = 2;
        tag = boo;
}

bool UBooType::iscompatible(UType * t)
{
        if(t->tag != boo) return false;
        return true;
}    


bool UEnuType::iscompatible(UType * t)
{
        if(t->tag != enu) return false;
	if(t != this) return false;
        return true;
}    

UValue * UEnuType::make()
{
	UEnuValue * val;
	val = new UEnuValue();
	val -> type = this;
	val -> v = 0;
	//val -> numeric = 0;
	return val;
}

UEnuType::UEnuType(UEnList * l)
{
	UEnList * ll;
	for(size = 0, ll = l; ll; ll = ll -> next,size++);
	tags = new EnSymbol * [size];
	for(unsigned int i = 0; i < size; i++)
	{
		tags[i] = l->sy;
		l -> sy -> ord = i;
		l -> sy -> type = this;
		l = l -> next;
	}
	tag = enu;
}

long unsigned int power(unsigned int a,unsigned int b)
{     
        long unsigned int c,i;
        c = 1;                                                                          for(i=0;i<b;i++)
        {
                c *= a;
        }
        return c;
} 
		
UArrType::UArrType(UType * in, UType * co)
{
	index = in;
	component = co;
	size = power(co -> size,in -> size);
	tag = arr;
}


UValue * UArrType::make()
{
	UArrValue * val;
	val = new UArrValue();
	val -> type = this;
	//val -> numeric = 0;
	val -> content = new UValue * [index -> size];
	for(unsigned int i = 0; i < index -> size; i++)
	{
		val -> content[i] = component -> make();
	}
	return val;
}

bool UArrType::iscompatible(UType * t)
{
        if(t->tag != arr) return false;
	if(((UArrType *) t)->index->size != index -> size) return false;
	if(component->iscompatible(((UArrType *) t)->component)) return(true);
        return false;
}    

		
//ULstType::ULstType(UType * co, unsigned int ml)
//{
	//component = co;
	//maxlength = ml;
	//size = 1;
	//for(int i = 1; i <= maxlength;i++)
	//{
		//size += power(co -> size,i);
	//}
//}

//UValue * ULstType::make()
//{
	//ULstValue * val;
	//val = new ULstValue();
	//val -> type = this;
	//val -> length = 0;
	//val -> numeric = 0;
	//val -> content = new UValue * [maxlength];
	//for(int i = 0; i < maxlength; i++)
	//{
		//val -> content[i] = (UValue *) 0;
	//}
//}

//bool ULstType::iscompatible(UType * t)
//{
        //if(t->tag != lst) return false;
	//if(component->iscompatible(((ULstType *) t)->component)) return(true);
        //return false;
//}    

		
URecType::URecType(URcList * l)
{
	URcList *ll;
	for(card = 0,ll = l;ll;ll = ll -> next,card++);
	tags = new RcSymbol * [card];
	component = new UType * [card ];
	size = 1;
	for(int i = card - 1; i >= 0; i--)
	{
		tags[i] = l -> sy;
		component[i] = l-> ty;
		size *= l -> ty -> size;
		l -> sy -> index = i;
		l = l -> next;
	}
	tag = rec;
}

UValue * URecType::make()
{
	URecValue * val;
	val = new URecValue();
	val -> type = this;
	//val -> numeric = 0;
	val -> content = new UValue * [card];
	for(int i = 0; i < card; i++)
	{
		val -> content[i] = component[i] -> make();
	}
	return val;
}

bool URecType::iscompatible(UType * t)
{
        if(t->tag != rec) return false;
	if(((URecType *) t)->card != card) return false;
	for(int i = 0; i < card ; i++)
	{
	if(!(component[i]->iscompatible(((URecType *) t)->component[i]))) return(false);
	}
        return true;
}    

		
//USetType::USetType(UType * co)
//{
	//component = co;
	//size = power(2,co -> size);
//}

//UValue * USetType::make()
//{
	//USetValue * val;
	//val = new USetValue();
	//val -> type = this;
	//val -> numeric = 0;
	//val -> content = new bool [component -> size];
	//for(int i = 0; i < component -> size; i++)
	//{
		//val -> content[i] = false;
	//}
//}

//bool USetType::iscompatible(UType * t)
//{
        //if(t->tag != set) return false;
	//if(component->iscompatible(((USetType *) t)->component)) return(true);
        //return false;
//}    

UValue * UNumValue::copy()
{
	UNumValue * val;
	val = new UNumValue();
	//val -> numeric = numeric;
	val -> type = type;
	val -> v = v;
	return val;
}

bool UNumValue::iseqqual(UValue * val)
{
	UNumValue * vv;

	if(val -> type -> tag != num) return false;
	vv = (UNumValue *) val;
	if(v == vv -> v) return true;
	return false;
}

bool UBooValue::iseqqual(UValue * val)
{
	UBooValue * vv;

	if(val -> type -> tag != boo) return false;
	vv = (UBooValue *) val;
	if(v == vv -> v) return true;
	return false;
}

bool UEnuValue::iseqqual(UValue * val)
{
	UEnuValue * vv;

	if(!type -> iscompatible(val-> type)) return false;
	vv = (UEnuValue *) val;
	if(v == vv -> v) return true;
	return false;
}

//bool ULstValue::iseqqual(UValue * val)
//{
	//ULstValue * vv;
//
	//if(! type -> iscompatible(val-> type)) return false;
	//vv = (ULstValue *) val;
	//if(length != vv -> length) return false;
	//for(int i=0;i<length; i++)
	//{
		//if(!content[i]->iseqqual(vv -> content[i])) return false;
	//}
	//return true;
//}

bool UArrValue::iseqqual(UValue * val)
{
	UArrValue * vv;

	if(!type -> iscompatible(val-> type)) return false;
	vv = (UArrValue *) val;
	for(unsigned i=0;i<((UArrType *) type) -> index -> size; i++)
	{
		if(!content[i]->iseqqual(vv -> content[i])) return false;
	}
	return true;
}

bool URecValue::iseqqual(UValue * val)
{
	URecValue * vv;

	if(! type -> iscompatible(val-> type)) return false;
	vv = (URecValue *) val;
	for(int i=0;i<((URecType *) type) -> card; i++)
	{
		if(!content[i]->iseqqual(vv -> content[i])) return false;
	}
	return true;
}

//bool USetValue::iseqqual(UValue * val)
//{
	//USetValue * vv;
	//UValue * left;
	//UValue * right;
//
	//int flag;
	//if(!type -> iscompatible(val-> type)) return false;
	//vv = (USetValue *) val;
	//left = ((USetType *) type) -> component -> make();
	//right = ((USetType *) (vv -> type)) -> component -> make();
	//do
	//{
		//if(vv -> content[right -> numeric])
		//{
			//// right is element of right set
			//flag = 0;
			//do
			//{
				//if((content[left->numeric]) && left -> iseqqual(right))
				//{
					//flag = 1;
					//break;
				//}
				//(*left)++;
			//} while(!left -> isfirst());
			//if(flag)
			//{
				//return false;
			//}
		//}
		//(*right)++;
	//} while(! right -> isfirst());
	//do
	//{
		//if(content[left -> numeric])
		//{
			//// left is element of left set
			//flag = 0;
			//do
			//{
				//if((vv -> content[right->numeric]) && right -> iseqqual(left))
				//{
					//flag = 1;
					//break;
				//}
				//(*right)++;
			//} while(!right -> isfirst());
			//if(flag)
			//{
				//return false;
			//}
		//}
		//(*left)++;
	//} while(! left -> isfirst());
//
//}


bool UNumValue::islesseqqual(UValue * val)
{
	UNumValue * vv;

	if(val -> type -> tag != num) return false;
	vv = (UNumValue *) val;
	if(v <= vv -> v) return true;
	return false;
}

bool UBooValue::islesseqqual(UValue * val)
{
	UBooValue * vv;

	if(val -> type -> tag != boo) return false;
	vv = (UBooValue *) val;
	if(v <= vv -> v) return true;
	return false;
}

bool UEnuValue::islesseqqual(UValue * val)
{
	UEnuValue * vv;

	if(!type -> iscompatible(val-> type)) return false;
	vv = (UEnuValue *) val;
	if(v <= vv -> v) return true;
	return false;
}

//bool ULstValue::islesseqqual(UValue * val)
//{
	//ULstValue * vv;

	//int l;
//
	//if(! type -> iscompatible(val-> type)) return false;
	//vv = (ULstValue *) val;
	//if(length < vv->length) l = length; else l = vv->length;
	//for(int i=0;i<l; i++)
	//{
		//if(!content[i]->islesseqqual(vv -> content[i])) return false;
		//if(!content[i]->iseqqual(vv -> content[i])) return true;
	//}
	//if(length == l) return true;
	//return false;
//}

bool UArrValue::islesseqqual(UValue * val)
{
	UArrValue * vv;

	if(!type -> iscompatible(val-> type)) return false;
	vv = (UArrValue *) val;
	for(unsigned int i=0;i<((UArrType *) type) -> index -> size; i++)
	{
		if(!content[i]->islesseqqual(vv -> content[i])) return false;
	}
	return true;
}

bool URecValue::islesseqqual(UValue * val)
{
	URecValue * vv;

	if(! type -> iscompatible(val-> type)) return false;
	vv = (URecValue *) val;
	for(int i=0;i<((URecType *) type) -> card; i++)
	{
		if(!content[i]->islesseqqual(vv -> content[i])) return false;
	}
	return true;
}

//bool USetValue::islesseqqual(UValue * val)
//{
	//USetValue * vv;
	//UValue * left;
	//UValue * right;
//
	//int flag;
	//if(!type -> iscompatible(val-> type)) return false;
	//vv = (USetValue *) val;
	//left = ((USetType *) type) -> component -> make();
	//right = ((USetType * ) (vv -> type)) -> make();
	//do
	//{
		//if(content[left -> numeric])
		//{
			//// left is element of left set
			//flag = 0;
			//do
			//{
				//if((vv -> content[right->numeric]) && right -> iseqqual(left))
				//{
					//flag = 1;
					//break;
				//}
				//(*right)++;
			//} while(!right -> isfirst());
			//if(flag)
			//{
				//return false;
			//}
		//}
		//(*left)++;
	//} while(! left -> isfirst());
//
//}


void UNumValue::assign(UValue * val)
{
	v = ((UNumValue *) val) -> v;
	while(v < ((UNumType *) type) -> lower)
	{
		v += ((UNumType *) type) -> size;
	}
	while(v > ((UNumType *) type) -> upper)
	{
		v -= ((UNumType *) type) -> size;
	}
}

void UBooValue::assign(UValue * val)
{
	v = ((UBooValue *) val) -> v;
	//numeric = ((UBooValue *) val) -> numeric;
}

void UEnuValue::assign(UValue * val)
{
	v = ((UEnuValue *) val) -> v;
	//numeric = ((UEnuValue *) val) -> numeric;
}

//void ULstValue::assign(UValue * val)
//{
//
	//ULstValue * lst;
	//int i;
	//lst = (ULstValue *) val;
	//numeric = 0;
	//int length;
	//if(lst -> length < ((ULstType *) type) -> maxlength)
	//{
		//length = lst -> length;
	//}
	//else
	//{
		//length = ((ULstType *) type) -> maxlength;
	//}
	//for(i=0;i< length; i++)
	//{
		//numeric += power(((ULstType *) type) -> component -> size,i);
	//}
	//int pot = 1;
	//for(i = length -1; i >= 0; i--)
	//{
		//content[i] -> assign(lst->content[i]);
		//numeric += content[i]->numeric * pot;
		//pot *= ((ULstType *) type) -> component -> size ;
	//}	
//}

void UArrValue::assign(UValue * val)
{
	UArrValue * arr;
	arr = (UArrValue *) val;
	//numeric = 0;
	//int pot = 1;
	for(int i = ((UArrType *) type) -> index -> size -1; i >= 0; i--)
	{
		content[i]-> assign(arr->content[i]);
		//numeric += content[i]->numeric * pot;
		//pot *= ((UArrType *) type) -> component -> size ;
	}	
	
}

char * UNumValue::text()
{
	unsigned int i, len;
	char * c;

	for(len = 0, i = v; i > 0 ; len ++, i /= 10);
	c = new char [len + 1];
	sprintf(c,"%u",v);
	return c;
}

char * UBooValue::text()
{
	char * c;

	if(v)
	{
		c = new char[5];
		c = strcpy(c,"TRUE");
	}
	else
	{
		c = new char[6];
		c = strcpy(c,"FALSE");
	}
	return c;
}

char * UEnuValue::text()
{
	char * c;

	c = new char [ strlen(((UEnuType *) type) -> tags[v] -> name)];
	strcpy(c,((UEnuType *) type) -> tags[v] -> name);
	return c;
}

char * UArrValue::text()
{
	char * c;
	unsigned int i;
	unsigned int len;

	char ** cc;

	cc = new char * [((UArrType *) type)->index->size];
	len = 1;
	for(i = 0; i < ((UArrType *) type)->index->size; i++)
	{
		cc[i] = content[i]->text();
		len += strlen(cc[i]) + 1;
	}
	c = new char [len + 2];
	strcpy(c,"[");
	len = 1;
	for(i=0; i < ((UArrType *) type)->index->size; i++)
	{
		strcpy(c + len, cc[i]);
		len +=strlen(cc[i]);
		delete [] cc[i];
		strcpy(c + len, "|");
		len++;
	}
	delete [] cc;
	strcpy(c + len - 1,"]");
	return c;
}

char * URecValue::text()
{
	char * c;
	int i;
	unsigned int len;

	char ** cc;

	cc = new char * [((URecType *) type)->card];
	len = 1;
	for(i = 0; i < ((URecType *) type)->card; i++)
	{
		cc[i] = content[i]->text();
		len += strlen(cc[i]) + 1;
	}
	c = new char [len + 2];
	strcpy(c,"<");
	len = 1;
	for(i=0; i < ((URecType *) type)->card; i++)
	{
		strcpy(c + len, cc[i]);
		len +=strlen(cc[i]);
		delete [] cc[i];
		strcpy(c + len, "|");
		len++;
	}
	delete [] cc;
	strcpy(c + len - 1,">");
	return c;
}





//void USetValue::assign(UValue * val)
//{
	//USetValue * set;
	//UValue * foreigncomponent;
	//UValue * mycomponent;
	//int i;
//	
	//set = (USetValue *) val;
	//numeric = 0;
//
	//foreigncomponent = ((USetType *) (set -> type)) -> component -> make();
	//mycomponent = ((USetType *) type) -> component -> make();
//	
	//for(i=0;i < ((USetType *) type) -> component -> size;i++)
	//{
		//content[i] = false;
	//}
	//do
	//{
		//if(set -> content[foreigncomponent -> numeric])
		//{
			//mycomponent -> assign(foreigncomponent);
			//if(content[mycomponent -> numeric] == false)
			//{
				//content[mycomponent -> numeric] = true;
				//numeric += power(2,mycomponent -> numeric);
			//}
		//}
		//(* foreigncomponent) ++;
	//}
	//while(!(foreigncomponent -> isfirst()));
//}

void URecValue::assign(UValue * val)
{
	URecValue * rec;
	rec = (URecValue *) val;
	//numeric = 0;
	//int pot = 1;
	for(int i = ((URecType *) type) -> card -1; i >= 0; i--)
	{
		content[i]-> assign(rec->content[i]);
		//numeric += content[i]->numeric * pot;
		//pot *= ((URecType *) type) -> component[i] -> size ;
	}	
	
}

//bool UValue::isfirst()
//{ 
	//if(numeric) return false;
	//return true;
//}

void UNumValue::operator ++ (int)
{
	v ++; 
	if(v > ((UNumType *) type) -> upper)
	{
		 v = ((UNumType *) type) -> lower;
	}
}

void UNumValue::print(ostream & os)
{
	os << v;
}
	
void UBooValue::print(ostream & os)
{
	if(v == false)
	{
		os << "FALSE";
	}
	else
	{
		os << "TRUE";
	}
}

UValue * UBooValue::copy()
{
	UBooValue * val;
	val = new UBooValue();
	val -> type = type;
	//val -> numeric = numeric;
	val -> v = v;
	return val;
}

void UBooValue::operator ++ (int)
{
	if(v)
	{
		 v = false; 
		 //numeric = false;
        }
	else 
	{
		v = true;
		//numeric = true;
	}
}
 
 UValue * UEnuValue::copy()
 {
 	UEnuValue * val;
	val = new UEnuValue();
	val -> v = v;
	val -> type = type;
	//val -> numeric = numeric;
	return val;
}
 	
void UEnuValue::operator ++ (int)
{
	v ++;  //numeric ++;
	if(v >= ((UEnuType *) type) -> size)
	{
		 v = 0;
		 //numeric = 0;
	}
}

void UEnuValue::print(ostream & os)
{
	os << ((UEnuType *) type) -> tags[v] -> name;
}
 
UValue * UArrValue::copy()
{
	UArrValue * val;
	val = new UArrValue();
	//val -> numeric = numeric;
	val -> type = type;
	val -> content = new UValue * [((UArrType*) type)->index -> size];
	for(unsigned int i = 0; i < ((UArrType*) type)->index -> size; i++)
	{
		val -> content[i] = content[i]->copy();
	}
	return val;
}

void UArrValue::operator ++ (int)
{
	//numeric++;
	//if(numeric >= ((UArrType *) type) -> size) numeric = 0;
	for (int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		(*(content[i]))++;
		if(!(content[i]->isfirst())) break;
	}
}

void UArrValue::print(ostream & os)
{
	os << "[";
	for(unsigned int i = 0; i< ((UArrType *) type)->component->size - 1;i++)
	{
		os << content[i] << "|";
	}
	os << content[((UArrType *) type)->component->size - 1] << "]";
}
 
 
//UValue * ULstValue::copy()
//{
	//ULstValue * val;
	//val = new ULstValue();
	//val -> numeric = numeric;
	//val -> type = type;
	//val -> content = new UValue * [((ULstType*) type)->maxlength];
	//int i;
	//for(i = 0; i < length; i++)
	//{
		//val -> content[i] = content[i]->copy();
	//}
	//for(; i < ((ULstType*) type)->maxlength; i++)
	//{
		//val -> content[i] = (UValue *) 0;
	//}
//}

//void ULstValue::operator ++ (int)
//{
	//numeric++;
	//if(numeric >= ((ULstType *) type) -> size) numeric = 0;
	//for (int i = length - 1; i >= 0; i--)
	//{
		//(*(content[i]))++;
		//if(!(content[i]->isfirst())) break;
	//}
	//if(length >= ((ULstType *) type )-> maxlength)
	//{	
		//return;
	//}
	//content[length] = ((ULstType *) type) -> make();
	//length ++;
//}
//
//void ULstValue::print(ostream & os)
//{
	//os << "`";
	//for(int i = 0; i< length - 1;i++)
	//{
		//os << content[i] << "/";
	//}
	//os << content[length - 1] << "´";
//}
 
 
UValue * URecValue::copy()
{
	URecValue * val;
	val = new URecValue();
	//val -> numeric = numeric;
	val -> type = type;
	val -> content = new UValue * [((URecType*) type)->card];
	for(int i = 0; i < ((URecType*) type)->card; i++)
	{
		val -> content[i] = content[i]->copy();
	}
	return val;
}

void URecValue::operator ++ (int)
{
	//numeric++;
	//if(numeric >= ((URecType *) type) -> size) numeric = 0;
	for (int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		(*(content[i]))++;
		if(!(content[i]->isfirst())) break;
	}
}

void URecValue::print(ostream & os)
{
	os << "<";
	for(int i = 0; i< ((URecType *) type)->card - 1;i++)
	{
		os << content[i] << "|";
	}
	os << content[((URecType *) type)->card - 1] << ">";
}
 
 
//UValue * USetValue::copy()
//{
	//USetValue * val;
	//val = new USetValue();
	//val -> numeric = numeric;
	//val -> type = type;
	//val -> content = new bool [((USetType*) type)->component -> size];
	//for(int i = 0; i < ((USetType*) type)->component -> size; i++)
	//{
		//val -> content[i] = content[i];
	//}
//}
//
//void USetValue::operator ++ (int)
//{
	//numeric++;
	//if(numeric >= ((USetType *) type) -> size) numeric = 0;
	//for (int i = ((USetType *) type) -> component -> size - 1; i >= 0; i--)
	//{
		//if(content[i] == false)
		//{
			//content[i] = true;
			//break;
		//}
		//content[i] = false;
	//}
//}
//
//void USetValue::print(ostream & os)
//{
	//int second;
	//second = 0;
	//os << "{";
	//for(int i = 0; i< ((USetType *) type)->component->size - 1;i++)
	//{
		//if(content[i])
		//{
			//UValue * val;
			//val = ((USetType *) type) -> make();
			//for(int j = 0; j < i; j ++)
			//{
				//(* val)++;
			//}
			//if(second)
			//{
				//os << "/";
				//os << val;
			//}
			//second++;
		//}
	//}
	//os << "}";
//}
 
void UWhileStatement::execute()
{
	while( ((UBooValue *) (cond -> evaluate()))-> v == true )
	{
		body -> execute();
	}
}

void URepeatStatement::execute()
{
	do
	{
		body -> execute();
	}
	while( ((UBooValue *) (cond -> evaluate()))-> v != true );
}

void UAssignStatement::execute()
{
	UValue * val;
	UValue * lval;
	//DerefList * rl;
	
	val = right -> evaluate();
	lval = left -> dereference();
	lval -> assign(val);
	//rl -> correct();
}

void UReturnStatement::execute()
{
	UValue * val;
	UValue * res;
	res = fct -> type -> make();
	val = exp -> evaluate();
	res -> assign(val); 
	fct -> addresult(res);
}

void UConditionalStatement::execute()
{
	UValue * val;
	val = cond -> evaluate();
	if(((UBooValue *) val) -> v == false)
	{
		no-> execute();
	}
	else
	{
		yes -> execute();
	}
}

void UCaseStatement::execute()
{
	int i;
	UValue * val;
	UValue * vval;

	vval = exp -> evaluate();
	for(i=0;i<card;i++)
	{
		val = cond[i]-> evaluate();
		if(vval -> iseqqual(val))
		{
			yes[i]-> execute();
			return;
		}
	}
	def -> execute();
}

//void UForinStatement::execute()
//{
	//UValue * first;
	//USetValue * s;
//
	//s = (USetValue *) set;
//
	//first = var -> type -> make();
	//var -> assign(first);
	//do
	//{
		//if(s -> content[(var -> get())->numeric] == true) body -> execute();
		//(*(var -> get()))++;
	//}
	//while((var -> get())->isfirst());
//}

void UForallStatement::execute()
{
	UValue * first;

	first = var -> type -> make();
	var -> assign(first);
	do
	{
		body -> execute();
		(*(var -> get()))++;
	}
	while(!((var -> get())->isfirst()));
}

void UForStatement::execute()
{
	UValue * first;
	UValue * last;

	last = finit -> evaluate();
	var -> assign(last);
	last = (var -> get())-> copy();
	first = init -> evaluate();
	var -> assign(first);
	while(!((var->get()) -> iseqqual(last)))
	{
		body -> execute();
		(*(var -> get()))++;
	}
	body -> execute(); // execute body for finit value itself
}

void UExitStatement::execute()
{
	throw ExitException();
}

void USequenceStatement::execute()
{
	first -> execute();
	second -> execute();
}
	
void UFunction::addresult(UValue * va)
{
	UValueList * v;
	v = new UValueList;
	v -> next = result;
	v -> val = va;
	result = v;
}

void runtimeerror(char const * mess)
{
  fprintf(stderr, "lola: run time error: '%s'\n", mess);
	exit(3);
}

UValue *  UCallExpression::evaluate()
{
	// unused: UValueList * tmp;
	UValue * resu;
	fct -> enter();
	for(int i = 0; i < fct -> arity; i++)
	{
		(fct->formalpar)[i]->assign((currentpar[i])->evaluate());
	}
	try{
	fct -> body -> execute();
	}
	catch(ExitException){}
	if(!fct -> result) runtimeerror("function returned without result");
	if(fct -> result -> next) runtimeerror("function returned with multiple results");
	resu = fct -> result -> val;
	fct -> leave();
	return resu;
}	

	
UValue * UUneqqualExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue *) (TheBooType -> make());
	if(l -> iseqqual(r))
	{
		ret -> v = false;
	}
	else
	{
		ret -> v = true;
	}
	return ret;
}
	
UValue * UEqualExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue *) (TheBooType -> make());
	if(l -> iseqqual(r))
	{
		ret -> v = true;
	}
	else
	{
		ret -> v = false;
	}
	return ret;
}
	
//UValue * UInExpression::evaluate()
//{
	//UValue * l;
	//USetValue * r;
	//UBooValue * ret;
	//UType * bt;
	//UValue * test;
//
	//l = left -> evaluate();
	//r = (USetValue *) right -> evaluate();
	//bt = UBooType();
	//ret = (UBooValue* ) bt -> make();
	//test = ((USetType *) (r -> type)) -> component -> make();
	//do
	//{
		//if(r -> content[test -> numeric] && l->iseqqual(test))
		//{
			//ret -> v = true;
			//return ret;
		//}
		//(*test) ++;
	//} while(test -> isfirst());
//}

UValue * UGreaterthanExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue *) (TheBooType -> make());
	if((r -> islesseqqual(l)) && ! (l -> iseqqual(r)))
	{
		ret -> v = true;
	}
	else
	{
		ret -> v = false;
	}
	return ret;
}

UValue * ULesseqqualExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue *) (TheBooType -> make());
	if(l -> islesseqqual(r))
	{
		ret -> v = true;
	}
	else
	{
		ret -> v = false;
	}
	return ret;
}


UValue * UGreatereqqualExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue * ) (TheBooType -> make());
	if(r -> islesseqqual(l))
	{
		ret -> v = true;
	}
	else
	{
		ret -> v = false;
	}
	return ret;
}

UValue * ULessthanExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UBooValue * ret;

	l = left -> evaluate();
	r = right -> evaluate();
	ret = (UBooValue *) (TheBooType -> make());
	if(l -> islesseqqual(r) && !(l -> iseqqual(r)))
	{
		ret -> v = true;
	}
	else
	{
		ret -> v = false;
	}
	return ret;
}

//UValue * UPrefixExpression::evaluate()
//{
	//ULstValue * l;
	//ULstValue * r;
	//UBooValue * ret;
	//UType * bt;
//
	//l = (ULstValue *) left -> evaluate();
	//r = (ULstValue *) right -> evaluate();
	//bt = new UBooType();
	//ret = (UBooValue* ) bt -> make();
//
	//if(! l -> type -> iscompatible(r -> type)) return false;
	//if(l -> length > r->length)
	//{
		//ret -> v = false;
		//return ret;
	//}
	//for(int i=0;i<l->length; i++)
	//{
		//if(! l -> content[i]->iseqqual(r -> content[i]))
		//{
			//ret -> v =  false;
			//return ret;
		//}
	//}
	//ret -> v = true;
	//return ret;
//}


UValue * UNumValue::mulop(UValue * scd)
{
	UNumValue * s;
	s = (UNumValue *) scd;
	UNumValue * ret;

	ret = (UNumValue *) type -> make();
	ret -> v = v * s -> v;
	return ret;
}

UValue * UNumValue::divop(UValue * scd)
{
	UNumValue * s;
	s = (UNumValue *) scd;
	UNumValue * ret;

	if(s -> v == 0) runtimeerror("division by zero");
	ret = (UNumValue *) type -> make();
	ret -> v = v / s -> v;
	return ret;
}

UValue * UNumValue::modop(UValue * scd)
{
	UNumValue * s;
	s = (UNumValue *) scd;
	UNumValue * ret;

	ret = (UNumValue *) type -> make();
	if(s -> v == 0) runtimeerror("division by zero");
	ret -> v = v % s -> v;
	return ret;
}

UValue * UNumValue::subop(UValue * scd)
{
	UNumValue * s;
	s = (UNumValue *) scd;
	UNumValue * ret;

	ret = (UNumValue *) type -> make();
	ret -> v = v - s -> v;
	return ret;
}

UValue * UNumValue::negop()
{
	UNumValue * ret;

	ret = (UNumValue *) type -> make();
	ret -> v =  - v;
	return ret;
}

UValue * UBooValue::negop()
{
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(v)
	{
		ret -> v = false;
		return ret;
	}
	ret -> v = true;
	return ret;
}

UValue * UNumValue::addop(UValue * scd)
{
	UNumValue * s;
	s = (UNumValue *) scd;
	UNumValue * ret;

	ret = (UNumValue *) type -> make();
	ret -> v = v + s -> v;
	return ret;
}

UValue * UBooValue::mulop(UValue * scd) // AND
{
	UBooValue * s;
	s = (UBooValue *) scd;
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(!( v))
	{	
		ret -> v = false;
		//ret -> numeric = 1;
		return ret;
	}
	ret -> v = s -> v;
	//ret -> numeric = s -> numeric;
	return ret;
}

UValue * UBooValue::divop(UValue * scd) // IFF
{
	UBooValue * s;
	s = (UBooValue *) scd;
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(s -> v == v)
	{	
		ret -> v = true;
		//ret -> numeric = 1;
		return ret;
	}
	ret -> v = false;
	//ret -> numeric = s -> numeric;
	return ret;
}

UValue * UBooValue::modop(UValue * scd) // XOR
{
	UBooValue * s;
	s = (UBooValue *) scd;
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(s -> v != v)
	{	
		ret -> v = true;
		//ret -> numeric = 1;
		return ret;
	}
	ret -> v = false;
	//ret -> numeric = s -> numeric;
	return ret;
}

UValue * UBooValue::subop(UValue * scd) // IMPLIES
{
	UBooValue * s;
	s = (UBooValue *) scd;
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(s -> v)
	{	
		ret -> v = true;
		//ret -> numeric = 1;
		return ret;
	}
	if(!v)
	{
		ret -> v = true;
		return ret;
	}
	ret -> v = false;
	//ret -> numeric = s -> numeric;
	return ret;
}

UValue * UBooValue::addop(UValue * scd) // OR
{
	UBooValue * s;
	s = (UBooValue *) scd;
	UBooValue * ret;

	ret = (UBooValue *) type -> make();
	if(v)
	{	
		ret -> v = true;
		//ret -> numeric = 1;
		return ret;
	}
	ret -> v = s -> v;
	//ret -> numeric = s -> numeric;
	return ret;
}

UValue * UArrValue::negop()
{
	UArrValue * ret;

	ret = (UArrValue *) type -> make();
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> negop();
	}
	return ret;
}

UValue * UArrValue::mulop(UValue * scd)
{
	UArrValue * s;
	s = (UArrValue *) scd;
	UArrValue * ret;

	ret = (UArrValue *) type -> make();
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> mulop(s -> content[i]);
	}
	return ret;
}

UValue * UArrValue::divop(UValue * scd)
{
	UArrValue * s;
	s = (UArrValue *) scd;
	UArrValue * ret;

	ret = (UArrValue *) type -> make();
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> divop(s -> content[i]);
	}
	return ret;
}

UValue * UArrValue::modop(UValue * scd)
{
	UArrValue * s;
	s = (UArrValue *) scd;
	UArrValue * ret;

	ret = (UArrValue *) type -> make();
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> modop(s -> content[i]);
	}
	return ret;
}

UValue * UArrValue::subop(UValue * scd)
{
	UArrValue * s;
	s = (UArrValue *) scd;
	UArrValue * ret;

	ret = (UArrValue *) type -> make();
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> subop(s -> content[i]);
	}
	return ret;
}

UValue * UArrValue::addop(UValue * scd)
{
	UArrValue * s;
	s = (UArrValue *) scd;
	UArrValue * ret;
	//unsigned int pot;

	ret = (UArrValue *) type -> make();
	//ret -> numeric = 0;
	//pot = 1;
	for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> addop(s -> content[i]);
		//ret -> numeric += ret -> content[i]->numeric * pot;
		//pot *= ((UArrType*) (ret -> type)) -> component -> size;
	}
	return ret;
}

//UValue * ULstValue::addop(UValue * scd)
//{
	//ULstValue * s;
	//s = (ULstType *) scd;
	//ULstValue * ret;
	//unsigned int pot;
//
	//ret = ((ULstType *) type) -> make();
	//ret -> numeric = 0;
	//pot = 1;
	//for(int i = ((UArrType *) type) -> index -> size - 1; i >= 0; i--)
	//{
		//ret -> content[i] = content[i] -> addop(s -> content[i]);
		//ret -> numeric += ret -> content[i]->numeric * pot;
		//pot *= ((UArrType) (ret -> type)) -> component -> size;
	//}
	//return ret;
//}

UValue * URecValue::negop()
{
	URecValue * ret;

	ret = (URecValue *) type-> make();
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> negop();
	}
	return ret;
}

UValue * URecValue::addop(UValue * scd)
{
	URecValue * s;
	s = (URecValue *) scd;
	URecValue * ret;

	ret = (URecValue *) type-> make();
	//pot = 1;
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> addop(s -> content[i]);
	}
	return ret;
}

UValue * URecValue::subop(UValue * scd)
{
	URecValue * s;
	s = (URecValue *) scd;
	URecValue * ret;

	ret = (URecValue *) type-> make();
	//pot = 1;
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> subop(s -> content[i]);
	}
	return ret;
}

UValue * URecValue::mulop(UValue * scd)
{
	URecValue * s;
	s = (URecValue *) scd;
	URecValue * ret;

	ret = (URecValue *) type-> make();
	//pot = 1;
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> mulop(s -> content[i]);
	}
	return ret;
}

UValue * URecValue::divop(UValue * scd)
{
	URecValue * s;
	s = (URecValue *) scd;
	URecValue * ret;

	ret = (URecValue *) type-> make();
	//pot = 1;
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> divop(s -> content[i]);
	}
	return ret;
}

UValue * URecValue::modop(UValue * scd)
{
	URecValue * s;
	s = (URecValue *) scd;
	URecValue * ret;
	//unsigned int pot;

	ret = (URecValue *) type-> make();
	//ret -> numeric = 0;
	//pot = 1;
	for(int i = ((URecType *) type) -> card - 1; i >= 0; i--)
	{
		ret -> content[i] = content[i] -> modop(s -> content[i]);
		//ret -> numeric += ret -> content[i]->numeric * pot;
		//pot *= ((URecType *) (ret -> type)) -> component[i] -> size;
	}
	return ret;
}

bool UNumValue::isfirst()
{
	if(v == ((UNumType *) type) -> lower) return true;
	return false;
}

bool UBooValue::isfirst()
{
	if(v == false) return true;
	return false;
}

bool UEnuValue::isfirst()
{
	if(v == 0) return true;
	return false;
}

bool UArrValue::isfirst()
{
	for(unsigned int i = 0; i < ((UArrType *) type) -> index -> size; i++)
	{
		if (!(content[i]->isfirst())) return false;
	}
	return true;
}

bool URecValue::isfirst()
{
	for(int i=0; i < ((URecType *) type) -> card; i++)
	{
		if(!(content[i]->isfirst())) return false;
	}
	return true;
}

UValue * USubExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UValue * res;

	l = left -> evaluate();
	r = right -> evaluate();
	res = l -> subop(r);
	return res;
}
	
UValue * UMulExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UValue * res;

	l = left -> evaluate();
	r = right -> evaluate();
	res = l -> mulop(r);
	return res;
}
	
UValue * UDivExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UValue * res;

	l = left -> evaluate();
	r = right -> evaluate();
	res = l -> divop(r);
	return res;
}
	
UValue * UModExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UValue * res;

	l = left -> evaluate();
	r = right -> evaluate();
	res = l -> modop(r);
	return res;
}
	
UValue * UNegExpression::evaluate()
{
	UValue * l;
	UValue * res;

	l = left -> evaluate();
	res = l -> negop();
	return res;
}

UValue * UAddExpression::evaluate()
{
	UValue * l;
	UValue * r;
	UValue * res;

	l = left -> evaluate();
	r = right -> evaluate();
	res = l -> addop(r);
	return res;
}

UValue * UTrueExpression::evaluate()
{
	UValue * val;

	val = TheBooType -> make();
	((UBooValue *) val) -> v = true;
	return val;
}

UValue * UFalseExpression::evaluate()
{
	UValue * val;

	val = TheBooType -> make();
	((UBooValue *) val) -> v = false;
	return val;
}

UValue * UEnuconstantExpression::evaluate()
{
	UValue * val;

	val = type -> make();
	((UEnuValue *) val) -> v = nu;
	return val;
}

UValue * UIntconstantExpression::evaluate()
{
	UValue * val;

	val = TheNumType -> make();
	((UNumValue *) val) -> v = nu;
	return val;
}

UValue * ULvalExpression::evaluate()
{
	UValue * ref;
	ref = lval -> dereference();
	return ref -> copy();
}

UValue * URecordExpression::evaluate()
{
	URecValue * a;
	URecType * t;
	UValue ** c;
	unsigned int i;
	URcList * rc;
	URcList * rc1;

	rc = (URcList *) 0;
	c = new UValue * [card];
	for(i = 0; i < card; i++)
	{
		c[i] = cont[i] -> evaluate();
		rc1 = new URcList;
		rc1 -> next = rc;
		rc1 -> ty = c[i] -> type;
		rc1 -> sy = (RcSymbol *) 0;
		rc = rc1;
	}

	t = new URecType(rc);
	a = (URecValue *) (t -> make());
	for(i=0;i<card;i++)
	{
		a -> content[i] -> assign(c[i]);
		delete c[i];
	}
	delete [] c;
	return a;
}

		
	
UValue * UArrayExpression::evaluate()
{
	UArrValue * a;
	UArrType * t;
	UNumType * it;
	UType * ct;
	UValue ** c;
	unsigned int i;

	it = new UNumType(1,card);
	c = new UValue * [card];
	for(i = 0; i < card; i++)
	{
		c[i] = cont[i] -> evaluate();
	}
	ct = c[0]->type;
	t = new UArrType(it, c[0] -> type);
	a = (UArrValue *) (t -> make());
	for(i=0;i<card;i++)
	{
		a -> content[i] -> assign(c[i]);
		delete c[i];
	}
	delete [] c;
	return a;
}

UValue * UVarLVal::dereference()
{
	return var -> get();
}

UValue * URecordLVal::dereference()
{
	UValue * v;

	v = parent -> dereference();
	return ((URecValue *) v) -> content[offset];
}

UValue * UArrayLVal::dereference()
{	
	UValue * v;
	UValue * in;
	unsigned int i;
	v = parent -> dereference();
	in = idx -> evaluate();
	if(indextype -> tag == boo)
	{
		if(((UBooValue *) in) -> v)
		{
			i = 1;
		}
		else
		{
			i = 0;
		}
	}
	else
	{
		i = (((UNumValue *) in) -> v - ((UNumType *) indextype) -> lower)
			  % indextype -> size ;
	}
	return ((UArrValue *) v) -> content[i];
}
		
UValue * UVar::get()
{
	return value;
}

void UVar::assign(UValue *v)
{
	value -> assign(v);
}

void UFunction::enter()
{
	//int i;
	UValueList * l;
	UResultList * r;
	UVar * va;
	Symbol *s;


	for(unsigned int i=0;i< localsymb -> size;i++)
	{
		for(s = localsymb -> table[i]; s ; s = s -> next)
		{
		va = ((VaSymbol *) s) -> var;
		l = new UValueList;
		l -> val = va->value;
		l -> next = va -> stack;
		va->value = va->type -> make();
		va->stack = l;
		}
	}
	//new result list
	r = new UResultList;
	r -> res = result;
	r -> next = resultstack;
	resultstack = r;

}
		
	
	
void UFunction::leave()
{
	// int i;
	UValueList * l;
	// not used: UResultList * r;
	UVar * va;
	Symbol * s;

	for(unsigned int i=0; i < localsymb -> size; i++)
	{
		for(s = localsymb -> table[i]; s; s = s -> next)
		{
				va = ((VaSymbol *) s) -> var;
				l = va -> stack;
//				delete va -> value;
				va ->value = l -> val;
				va -> stack = l -> next;;
				delete l;
			
		}
	}
	result = resultstack -> res;
	resultstack = resultstack -> next;
}
		
UVar::UVar(UType * t)
{
	type= t;
	stack = (UValueList *) 0;
	value = t -> make();
}

bool deep_compatible(UType * t1, UType * t2, UTypeClass t)
{
	if(! t1 -> iscompatible(t2)) return false;
	if(t1 -> tag == t && t2 -> tag == t) return true;
	if(t1 -> tag == num || t1 -> tag == boo || t1 -> tag == enu) return false;
	if(t1 -> tag == arr)
	{
		return deep_compatible(((UArrType *)  t1) -> component,((UArrType *) t2) -> component, t);
	}
	else
	{
		// t1 -> tag must be rec
		for(int i = 0; i < ((URecType *) t1) -> card; i++)
		{
			if(!deep_compatible(((URecType *) t1) -> component[i],((URecType *) t2) -> component[i], t)) return false;
		}
		return true;
	}
}

bool deep_compatible(UType * t1, UTypeClass t)
{
	if(t1 -> tag == t) return true;
	if(t1 -> tag == num || t1 -> tag == boo || t1 -> tag == enu) return false;
	if(t1 -> tag == arr)
	{
		return deep_compatible(((UArrType *)  t1) -> component, t);
	}
	else
	{
		// t1 -> tag must be rec
		for(int i = 0; i < ((URecType *) t1) -> card; i++)
		{
			if(!deep_compatible(((URecType *) t1) -> component[i], t)) return false;
		}
		return true;
	}
}

UValueList* UVarTerm::evaluate()
{
	UValueList * vl;
	vl = new UValueList;
	vl -> next = (UValueList *) 0;
	vl -> val = v -> get();
	char * c;
	c = vl -> val -> text();
	return vl;
}

UValueList * UOpTerm::evaluate()
{
	UValueList * vl;
	UValueList ** last;
	unsigned int i;
						char * bla;

	vl = (UValueList *) 0;

	UValueList ** subresult;
	UValueList ** subindex;

	subresult = new UValueList * [arity + 1] ;
	subindex = new UValueList * [arity +1 ] ;

	for(i=0;i < arity; i++)
	{
		subresult[i] = sub[i] -> evaluate();
		if(!(subresult[i])) return (UValueList *) 0;
		subindex[i] = subresult[i];
	}

	// Jetzt f fuer jede Kombination von Teilresultaten aufrufen
	while(1)
	{
		// 1. Funktionswert fuer derzeitige Argumentkombination
		f -> enter();
		for(i=0; i < arity; i++)
		{
			(f -> formalpar)[i]->assign((subindex[i])->val);
		}
		try{
		f -> body -> execute();
		}
		catch(ExitException){}
		for(last = &vl; * last; last = & ((* last) -> next));
		* last = f -> result;
		for(UValueList *vc = * last; vc; vc = vc -> next)
		{
			bla = vc -> val -> text();
		}
		f -> leave();

		// 2. Neue Argumentkombination
		if(!arity) 
		{
			return vl;
		}
		i = arity - 1;
		while(1)
		{
			if(subindex[i] -> next)
			{
				subindex[i] = subindex[i] -> next;
				break;
			}
			else
			{
				subindex[i] = subresult[i];
				if(i>0) 
				{
					i--;
					continue;
				}
				else
				{
					// keine weiteren Argumenttupel
					delete [] subindex;
					for(i=0; i < arity; i++)
					{
						UValueList * tmp;

						while(subresult[i])
						{
							tmp = subresult[i];
							subresult[i] = subresult[i] -> next;
							delete tmp;
						}
					}
					delete [] subresult;
					for(UValueList * vc = vl; vc; vc = vc -> next)
					{
						bla = vc -> val -> text();
					}
					return vl;
				}
			}
		}
	}
}


