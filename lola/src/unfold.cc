/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "net.H"
#include "dimensions.H"
#include "unfold.H"
#include "symboltab.H"
#include <cstring>
#include <cstdio>

extern void yyerrors(char* token, char const* format, ...);
extern char* yytext;

inline long unsigned int power(unsigned int a, unsigned int b) {
    long unsigned int c;
    c = 1;
    for (unsigned int i = 0; i < b; i++) {
        c *= a;
    }
    return c;
}

const char* getUTypeName(UTypeClass c) {
    switch (c) {
        case(arr):
            return "array";
        case(rec):
            return "record";
        case(num):
            return "integer";
        case(boo):
            return "Boolean";
        case(enu):
            return "enumeration";
    }
    return NULL;
}

UBooType* TheBooType;
UNumType* TheNumType;

UNumType::UNumType(int low, int up) {
    if (low > up) {
        yyerrors(yytext, "negative range in integer type definition");
    }
    lower = low;
    upper = up;
    size = up - low + 1;
    tag = num;
}

bool UNumType::iscompatible(UType* t) {
    if (t->tag != num) {
        return false;
    }
    return true;
}

UNumValue::UNumValue(UType* _type, int _v) : v(_v) {
    type = _type;
}

UValue* UNumType::make() {
    return new UNumValue(this, lower);
}

UBooValue::UBooValue(UType* _type, bool _v) : v(_v) {
    type = _type;
}

UValue* UBooType::make() {
    return new UBooValue(this, false);
}

UBooType::UBooType() {
    size = 2;
    tag = boo;
}

bool UBooType::iscompatible(UType* t) {
    return (t->tag == boo);
}


bool UEnuType::iscompatible(UType* t) {
    return (t->tag == enu || t == this);
}

UEnuValue::UEnuValue(UType* _type, int _v) : v(_v) {
    type = _type;
}

UValue* UEnuType::make() {
    return new UEnuValue(this, 0);
}

UEnuType::UEnuType(UEnList* l) {
    UEnList* ll;
    for (size = 0, ll = l; ll; ll = ll->next, size++) {
        ;
    }
    tags = new EnSymbol * [size];
    for (unsigned int i = 0; i < size; i++) {
        tags[i] = l->sy;
        l->sy->ord = i;
        l->sy->type = this;
        l = l->next;
    }
    tag = enu;
}

UEnList::UEnList() : sy(NULL), next(NULL) {}

UEnList::UEnList(EnSymbol* _sy) : sy(_sy), next(NULL) {}

UArrType::UArrType(UType* in, UType* co) {
    index = in;
    component = co;
    size = power(co->size, in->size);
    tag = arr;
}


UValue* UArrType::make() {
    UArrValue* val = new UArrValue();
    val->type = this;
    val->content = new UValue * [index->size];
    for (unsigned int i = 0; i < index->size; ++i) {
        val->content[i] = component->make();
    }
    return val;
}

bool UArrType::iscompatible(UType* t) {
    if (t->tag != arr or((UArrType*) t)->index->size != index->size) {
        return false;
    }
    return (component->iscompatible(((UArrType*) t)->component));
}

URcList::URcList(RcSymbol* _sy, UType* _ty, URcList* _next) : sy(_sy), ty(_ty), next(_next) {}

URecType::URecType(URcList* l) {
    URcList* ll;
    for (card = 0, ll = l; ll; ll = ll->next, card++) {
        ;
    }
    tags = new RcSymbol * [card];
    component = new UType * [card ];
    size = 1;
    for (int i = card - 1; i >= 0; i--) {
        tags[i] = l->sy;
        component[i] = l->ty;
        size *= l->ty->size;
        l->sy->index = i;
        l = l->next;
    }
    tag = rec;
}

UValue* URecType::make() {
    URecValue* val = new URecValue();
    val->type = this;
    val->content = new UValue * [card];
    for (int i = 0; i < card; i++) {
        val->content[i] = component[i]->make();
    }
    return val;
}

bool URecType::iscompatible(UType* t) {
    if (t->tag != rec) {
        return false;
    }
    if (((URecType*) t)->card != card) {
        return false;
    }
    for (int i = 0; i < card ; i++) {
        if (!(component[i]->iscompatible(((URecType*) t)->component[i]))) {
            return(false);
        }
    }
    return true;
}

UValue* UNumValue::copy() {
    return new UNumValue(type, v);
}

bool UNumValue::iseqqual(UValue* val) {
    if (val->type->tag != num) {
        return false;
    }
    UNumValue* vv = static_cast<UNumValue*>(val);
    return (v == vv->v);
}

bool UBooValue::iseqqual(UValue* val) {
    if (val->type->tag != boo) {
        return false;
    }
    UBooValue* vv = static_cast<UBooValue*>(val);
    return (v == vv->v);
}

bool UEnuValue::iseqqual(UValue* val) {
    if (!type->iscompatible(val->type)) {
        return false;
    }
    UEnuValue* vv = static_cast<UEnuValue*>(val);
    return (v == vv->v);
}

bool UArrValue::iseqqual(UValue* val) {
    if (!type->iscompatible(val->type)) {
        return false;
    }
    UArrValue* vv = static_cast<UArrValue*>(val);
    for (unsigned i = 0; i < (static_cast<UArrType*>(type))->index->size; i++) {
        if (!content[i]->iseqqual(vv->content[i])) {
            return false;
        }
    }
    return true;
}

bool URecValue::iseqqual(UValue* val) {
    if (! type->iscompatible(val->type)) {
        return false;
    }
    URecValue* vv = static_cast<URecValue*>(val);
    for (int i = 0; i < (static_cast<URecType*>(type))->card; i++) {
        if (!content[i]->iseqqual(vv->content[i])) {
            return false;
        }
    }
    return true;
}

bool UNumValue::islesseqqual(UValue* val) {
    if (val->type->tag != num) {
        return false;
    }
    UNumValue* vv = static_cast<UNumValue*>(val);
    return (v <= vv->v);
}

bool UBooValue::islesseqqual(UValue* val) {
    if (val->type->tag != boo) {
        return false;
    }
    UBooValue* vv = static_cast<UBooValue*>(val);
    return (v <= vv->v);
}

bool UEnuValue::islesseqqual(UValue* val) {
    if (!type->iscompatible(val->type)) {
        return false;
    }
    UEnuValue* vv = static_cast<UEnuValue*>(val);
    return (v <= vv->v);
}

bool UArrValue::islesseqqual(UValue* val) {
    if (!type->iscompatible(val->type)) {
        return false;
    }
    UArrValue* vv = static_cast<UArrValue*>(val);
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->index->size; i++) {
        if (!content[i]->islesseqqual(vv->content[i])) {
            return false;
        }
    }
    return true;
}

bool URecValue::islesseqqual(UValue* val) {
    if (! type->iscompatible(val->type)) {
        return false;
    }
    URecValue* vv = static_cast<URecValue*>(val);
    for (int i = 0; i < (static_cast<URecType*>(type))->card; i++) {
        if (!content[i]->islesseqqual(vv->content[i])) {
            return false;
        }
    }
    return true;
}

void UNumValue::assign(UValue* val) {
    v = (static_cast<UNumValue*>(val))->v;
    while (v < static_cast<UNumType*>(type)->lower) {
        v += static_cast<UNumType*>(type)->size;
    }
    while (v > static_cast<UNumType*>(type)->upper) {
        v -= static_cast<UNumType*>(type)->size;
    }
}

void UBooValue::assign(UValue* val) {
    v = (static_cast<UBooValue*>(val))->v;
}

void UEnuValue::assign(UValue* val) {
    v = (static_cast<UEnuValue*>(val))->v;
}

void UArrValue::assign(UValue* val) {
    UArrValue* arr = static_cast<UArrValue*>(val);
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        content[i]->assign(arr->content[i]);
    }
}

char* UNumValue::text() {
    unsigned int i, len;
    for (len = 0, i = v; i > 0 ; len ++, i /= 10) {
        ;
    }
    char* c = new char [len + 1];
    sprintf(c, "%u", v);
    return c;
}

char* UBooValue::text() {
    char* c;

    if (v) {
        c = new char[5];
        c = strcpy(c, "TRUE");
    } else {
        c = new char[6];
        c = strcpy(c, "FALSE");
    }
    return c;
}

char* UEnuValue::text() {
    char* c = new char [ strlen(((UEnuType*) type)->tags[v]->name)];
    strcpy(c, ((UEnuType*) type)->tags[v]->name);
    return c;
}

char* UArrValue::text() {
    char** cc = new char * [(static_cast<UArrType*>(type))->index->size];
    unsigned int len = 1;
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->index->size; i++) {
        cc[i] = content[i]->text();
        len += strlen(cc[i]) + 1;
    }
    char* c = new char [len + 2];
    strcpy(c, "[");
    len = 1;
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->index->size; i++) {
        strcpy(c + len, cc[i]);
        len += strlen(cc[i]);
        delete [] cc[i];
        strcpy(c + len, "|");
        len++;
    }
    delete [] cc;
    strcpy(c + len - 1, "]");
    return c;
}

char* URecValue::text() {
    char** cc = new char * [(static_cast<URecType*>(type))->card];
    unsigned int len = 1;
    for (int i = 0; i < (static_cast<URecType*>(type))->card; i++) {
        cc[i] = content[i]->text();
        len += strlen(cc[i]) + 1;
    }
    char* c = new char [len + 2];
    strcpy(c, "<");
    len = 1;
    for (int i = 0; i < (static_cast<URecType*>(type))->card; i++) {
        strcpy(c + len, cc[i]);
        len += strlen(cc[i]);
        delete [] cc[i];
        strcpy(c + len, "|");
        len++;
    }
    delete [] cc;
    strcpy(c + len - 1, ">");
    return c;
}

void URecValue::assign(UValue* val) {
    URecValue* rec = static_cast<URecValue*>(val);
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        content[i]->assign(rec->content[i]);
    }

}

void UNumValue::operator++ (int) {
    v++;
    if (v > static_cast<UNumType*>(type)->upper) {
        v = static_cast<UNumType*>(type)->lower;
    }
}

void UNumValue::print(ostream& os) {
    os << v;
}

void UBooValue::print(ostream& os) {
    os << (v ? "TRUE" : "FALSE");
}

UValue* UBooValue::copy() {
    return new UBooValue(type, v);
}

void UBooValue::operator++ (int) {
    v = !v;
}

UValue* UEnuValue::copy() {
    return new UEnuValue(type, v);
}

void UEnuValue::operator++ (int) {
    ++v;
    if ((unsigned int)v >= ((UEnuType*) type)->size) {
        v = 0;
    }
}

void UEnuValue::print(ostream& os) {
    os << ((UEnuType*) type)->tags[v]->name;
}

UValue* UArrValue::copy() {
    UArrValue* val = new UArrValue();
    val->type = type;
    val->content = new UValue * [(static_cast<UArrType*>(type))->index->size];
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->index->size; i++) {
        val->content[i] = content[i]->copy();
    }
    return val;
}

void UArrValue::operator++(int) {
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        (*(content[i]))++;
        if (!(content[i]->isfirst())) {
            break;
        }
    }
}

void UArrValue::print(ostream& os) {
    os << "[";
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->component->size - 1; ++i) {
        os << content[i] << "|";
    }
    os << content[(static_cast<UArrType*>(type))->component->size - 1] << "]";
}



UValue* URecValue::copy() {
    URecValue* val = new URecValue();
    val->type = type;
    val->content = new UValue * [(static_cast<URecType*>(type))->card];
    for (int i = 0; i < (static_cast<URecType*>(type))->card; i++) {
        val->content[i] = content[i]->copy();
    }
    return val;
}

void URecValue::operator++ (int) {
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        (*(content[i]))++;
        if (!(content[i]->isfirst())) {
            break;
        }
    }
}

void URecValue::print(ostream& os) {
    os << "<";
    for (int i = 0; i < (static_cast<URecType*>(type))->card - 1; i++) {
        os << content[i] << "|";
    }
    os << content[(static_cast<URecType*>(type))->card - 1] << ">";
}

void UWhileStatement::execute() {
    while (((UBooValue*)(cond->evaluate()))->v == true) {
        body->execute();
    }
}

void URepeatStatement::execute() {
    do {
        body->execute();
    } while (((UBooValue*)(cond->evaluate()))->v != true);
}

void UAssignStatement::execute() {
    UValue* val = right->evaluate();
    UValue* lval = left->dereference();
    lval->assign(val);
}

void UReturnStatement::execute() {
    UValue* res = fct->type->make();
    UValue* val = exp->evaluate();
    res->assign(val);
    fct->addresult(res);
}

void UConditionalStatement::execute() {
    UValue* val = cond->evaluate();
    if ((static_cast<UBooValue*>(val))->v == false) {
        no->execute();
    } else {
        yes->execute();
    }
}

void UCaseStatement::execute() {
    UValue* vval = exp->evaluate();
    for (int i = 0; i < card; ++i) {
        UValue* val = cond[i]->evaluate();
        if (vval->iseqqual(val)) {
            yes[i]->execute();
            return;
        }
    }
    def->execute();
}

void UForallStatement::execute() {
    UValue* first = var->type->make();
    var->assign(first);
    do {
        body->execute();
        (*(var->get()))++;
    } while (!((var->get())->isfirst()));
}

void UForStatement::execute() {
    UValue* last = finit->evaluate();
    var->assign(last);
    last = (var->get())->copy();
    UValue* first = init->evaluate();
    var->assign(first);
    while (!((var->get())->iseqqual(last))) {
        body->execute();
        (*(var->get()))++;
    }
    body->execute(); // execute body for finit value itself
}

void UExitStatement::execute() {
    throw ExitException();
}

void USequenceStatement::execute() {
    first->execute();
    second->execute();
}

void UFunction::addresult(UValue* va) {
    UValueList* v = new UValueList;
    v->next = result;
    v->val = va;
    result = v;
}

void runtimeerror(char const* mess) {
    yyerrors(yytext, "runtime error: %s", _cimportant_(mess));
}

UValue* UCallExpression::evaluate() {
    fct->enter();
    for (int i = 0; i < fct->arity; i++) {
        (fct->formalpar)[i]->assign((currentpar[i])->evaluate());
    }
    try {
        fct->body->execute();
    } catch (ExitException) {}
    if (!fct->result) {
        runtimeerror("function returned without result");
    }
    if (fct->result->next) {
        runtimeerror("function returned with multiple results");
    }
    UValue* resu = fct->result->val;
    fct->leave();
    return resu;
}


UValue* UUneqqualExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = (not l->iseqqual(r));
    return ret;
}

UValue* UEqualExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = l->iseqqual(r);
    return ret;
}

UValue* UGreaterthanExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = (r->islesseqqual(l) and not l->iseqqual(r));
    return ret;
}

UValue* ULesseqqualExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = l->islesseqqual(r);
    return ret;
}


UValue* UGreatereqqualExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = r->islesseqqual(l);
    return ret;
}

UValue* ULessthanExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UBooValue* ret = static_cast<UBooValue*>(TheBooType->make());
    ret->v = (l->islesseqqual(r) and not l->iseqqual(r));
    return ret;
}


UValue* UNumValue::mulop(UValue* scd) {
    UNumValue* s = static_cast<UNumValue*>(scd);
    UNumValue* ret = static_cast<UNumValue*>(type->make());
    ret->v = v * s->v;
    return ret;
}

UValue* UNumValue::divop(UValue* scd) {
    UNumValue* s = static_cast<UNumValue*>(scd);
    UNumValue* ret = static_cast<UNumValue*>(type->make());

    if (s->v == 0) {
        runtimeerror("division by zero");
    }

    ret->v = v / s->v;
    return ret;
}

UValue* UNumValue::modop(UValue* scd) {
    UNumValue* s = static_cast<UNumValue*>(scd);
    UNumValue* ret = static_cast<UNumValue*>(type->make());

    if (s->v == 0) {
        runtimeerror("division by zero");
    }

    ret->v = v % s->v;
    return ret;
}

UValue* UNumValue::subop(UValue* scd) {
    UNumValue* s = static_cast<UNumValue*>(scd);
    UNumValue* ret = static_cast<UNumValue*>(type->make());
    ret->v = v - s->v;
    return ret;
}

UValue* UNumValue::negop() {
    UNumValue* ret = static_cast<UNumValue*>(type->make());
    ret->v = - v;
    return ret;
}

UValue* UBooValue::negop() {
    UBooValue* ret = static_cast<UBooValue*>(type->make());
    ret->v = not v;
    return ret;
}

UValue* UNumValue::addop(UValue* scd) {
    UNumValue* s = static_cast<UNumValue*>(scd);
    UNumValue* ret = static_cast<UNumValue*>(type->make());
    ret->v = v + s->v;
    return ret;
}

UValue* UBooValue::mulop(UValue* scd) { // AND
    UBooValue* s = static_cast<UBooValue*>(scd);
    UBooValue* ret = static_cast<UBooValue*>(type->make());

    if (not v) {
        ret->v = false;
        return ret;
    }
    ret->v = s->v;
    return ret;
}

UValue* UBooValue::divop(UValue* scd) { // IFF
    UBooValue* s = static_cast<UBooValue*>(scd);
    UBooValue* ret = static_cast<UBooValue*>(type->make());
    ret->v = (s->v == v);
    return ret;
}

UValue* UBooValue::modop(UValue* scd) { // XOR
    UBooValue* s = static_cast<UBooValue*>(scd);
    UBooValue* ret = static_cast<UBooValue*>(type->make());

    ret->v = (s->v != v);
    return ret;
}

UValue* UBooValue::subop(UValue* scd) { // IMPLIES
    UBooValue* s = static_cast<UBooValue*>(scd);
    UBooValue* ret = static_cast<UBooValue*>(type->make());
    if (s->v or not v) {
        ret->v = true;
        return ret;
    }
    ret->v = false;
    return ret;
}

UValue* UBooValue::addop(UValue* scd) { // OR
    UBooValue* s = static_cast<UBooValue*>(scd);
    UBooValue* ret = static_cast<UBooValue*>(type->make());
    if (v) {
        ret->v = true;
        return ret;
    }
    ret->v = s->v;
    return ret;
}

UValue* UArrValue::negop() {
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->negop();
    }
    return ret;
}

UValue* UArrValue::mulop(UValue* scd) {
    UArrValue* s = static_cast<UArrValue*>(scd);
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->mulop(s->content[i]);
    }
    return ret;
}

UValue* UArrValue::divop(UValue* scd) {
    UArrValue* s = static_cast<UArrValue*>(scd);
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->divop(s->content[i]);
    }
    return ret;
}

UValue* UArrValue::modop(UValue* scd) {
    UArrValue* s = static_cast<UArrValue*>(scd);
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->modop(s->content[i]);
    }
    return ret;
}

UValue* UArrValue::subop(UValue* scd) {
    UArrValue* s = static_cast<UArrValue*>(scd);
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->subop(s->content[i]);
    }
    return ret;
}

UValue* UArrValue::addop(UValue* scd) {
    UArrValue* s = static_cast<UArrValue*>(scd);
    UArrValue* ret = static_cast<UArrValue*>(type->make());
    for (int i = (static_cast<UArrType*>(type))->index->size - 1; i >= 0; i--) {
        ret->content[i] = content[i]->addop(s->content[i]);
    }
    return ret;
}

UValue* URecValue::negop() {
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->negop();
    }
    return ret;
}

UValue* URecValue::addop(UValue* scd) {
    URecValue* s = static_cast<URecValue*>(scd);
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->addop(s->content[i]);
    }
    return ret;
}

UValue* URecValue::subop(UValue* scd) {
    URecValue* s = static_cast<URecValue*>(scd);
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->subop(s->content[i]);
    }
    return ret;
}

UValue* URecValue::mulop(UValue* scd) {
    URecValue* s = static_cast<URecValue*>(scd);
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->mulop(s->content[i]);
    }
    return ret;
}

UValue* URecValue::divop(UValue* scd) {
    URecValue* s = static_cast<URecValue*>(scd);
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->divop(s->content[i]);
    }
    return ret;
}

UValue* URecValue::modop(UValue* scd) {
    URecValue* s = static_cast<URecValue*>(scd);
    URecValue* ret = static_cast<URecValue*>(type->make());
    for (int i = (static_cast<URecType*>(type))->card - 1; i >= 0; i--) {
        ret->content[i] = content[i]->modop(s->content[i]);
    }
    return ret;
}

bool UNumValue::isfirst() {
    return (v == static_cast<UNumType*>(type)->lower);
}

bool UBooValue::isfirst() {
    return (v == false);
}

bool UEnuValue::isfirst() {
    return (v == 0);
}

bool UArrValue::isfirst() {
    for (unsigned int i = 0; i < (static_cast<UArrType*>(type))->index->size; ++i) {
        if (!(content[i]->isfirst())) {
            return false;
        }
    }
    return true;
}

bool URecValue::isfirst() {
    for (int i = 0; i < (static_cast<URecType*>(type))->card; ++i) {
        if (!(content[i]->isfirst())) {
            return false;
        }
    }
    return true;
}

UValue* USubExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UValue* res = l->subop(r);
    return res;
}

UValue* UMulExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UValue* res = l->mulop(r);
    return res;
}

UValue* UDivExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UValue* res = l->divop(r);
    return res;
}

UValue* UModExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UValue* res = l->modop(r);
    return res;
}

UValue* UNegExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* res = l->negop();
    return res;
}

UValue* UAddExpression::evaluate() {
    UValue* l = left->evaluate();
    UValue* r = right->evaluate();
    UValue* res = l->addop(r);
    return res;
}

UValue* UTrueExpression::evaluate() {
    UValue* val = TheBooType->make();
    (static_cast<UBooValue*>(val))->v = true;
    return val;
}

UValue* UFalseExpression::evaluate() {
    UValue* val = TheBooType->make();
    (static_cast<UBooValue*>(val))->v = false;
    return val;
}

UValue* UEnuconstantExpression::evaluate() {
    UValue* val = type->make();
    (static_cast<UEnuValue*>(val))->v = nu;
    return val;
}

UValue* UIntconstantExpression::evaluate() {
    UValue* val = TheNumType->make();
    (static_cast<UNumValue*>(val))->v = nu;
    return val;
}

UValue* ULvalExpression::evaluate() {
    UValue* ref = lval->dereference();
    return ref->copy();
}

UValue* URecordExpression::evaluate() {
    URcList* rc = NULL;
    UValue** c = new UValue * [card];
    for (unsigned int i = 0; i < card; ++i) {
        c[i] = cont[i]->evaluate();
        URcList* rc1 = new URcList(NULL, c[i]->type, rc);
        rc = rc1;
    }

    URecType* t = new URecType(rc); // leaks!
    URecValue* a = static_cast<URecValue*>(t->make());
    for (unsigned int i = 0; i < card; ++i) {
        a->content[i]->assign(c[i]);
        delete c[i];
    }
    delete [] c;
    return a;
}



UValue* UArrayExpression::evaluate() {
    UNumType* it = new UNumType(1, card);
    UValue** c = new UValue * [card];
    for (unsigned int i = 0; i < card; i++) {
        c[i] = cont[i]->evaluate();
    }
    UArrType* t = new UArrType(it, c[0]->type); // leaks!
    UArrValue* a = (UArrValue*)(t->make());
    for (unsigned int i = 0; i < card; i++) {
        a->content[i]->assign(c[i]);
        delete c[i];
    }
    delete [] c;
    return a;
}

UValue* UVarLVal::dereference() {
    return var->get();
}

UValue* URecordLVal::dereference() {
    UValue* v = parent->dereference();
    return static_cast<URecValue*>(v)->content[offset];
}

UValue* UArrayLVal::dereference() {
    unsigned int i;
    UValue* v = parent->dereference();
    UValue* in = idx->evaluate();
    if (indextype->tag == boo) {
        if (((UBooValue*) in)->v) {
            i = 1;
        } else {
            i = 0;
        }
    } else {
        i = (((UNumValue*) in)->v - ((UNumType*) indextype)->lower)
            % indextype->size ;
    }
    return ((UArrValue*) v)->content[i];
}

UValue* UVar::get() {
    return value;
}

void UVar::assign(UValue* v) {
    value->assign(v);
}

void UFunction::enter() {
    for (unsigned int i = 0; i < localsymb->size; i++) {
        for (Symbol* s = localsymb->table[i]; s ; s = s->next) {
            UVar* va = ((VaSymbol*) s)->var;
            UValueList* l = new UValueList;
            l->val = va->value;
            l->next = va->stack;
            va->value = va->type->make();
            va->stack = l;
        }
    }
    //new result list
    UResultList* r = new UResultList;
    r->res = result;
    r->next = resultstack;
    resultstack = r;
}



void UFunction::leave() {
    for (unsigned int i = 0; i < localsymb->size; i++) {
        for (Symbol* s = localsymb->table[i]; s; s = s->next) {
            UVar* va = ((VaSymbol*) s)->var;
            UValueList* l = va->stack;
            va->value = l->val;
            va->stack = l->next;;
            delete l;
        }
    }
    result = resultstack->res;
    resultstack = resultstack->next;
}

UVar::UVar(UType* t) {
    type = t;
    stack = NULL;
    value = t->make();
}

bool deep_compatible(UType* t1, UType* t2, UTypeClass t) {
    if (not t1->iscompatible(t2)) {
        return false;
    }
    if (t1->tag == t && t2->tag == t) {
        return true;
    }
    if (t1->tag == num || t1->tag == boo || t1->tag == enu) {
        return false;
    }
    if (t1->tag == arr) {
        return deep_compatible(static_cast<UArrType*>(t1)->component, static_cast<UArrType*>(t2)->component, t);
    } else {
        // t1->tag must be rec
        for (int i = 0; i < (static_cast<URecType*>(t1))->card; i++) {
            if (!deep_compatible((static_cast<URecType*>(t1))->component[i], static_cast<URecType*>(t2)->component[i], t)) {
                return false;
            }
        }
        return true;
    }
}

bool deep_compatible(UType* t1, UTypeClass t) {
    if (t1->tag == t) {
        return true;
    }
    if (t1->tag == num || t1->tag == boo || t1->tag == enu) {
        return false;
    }
    if (t1->tag == arr) {
        return deep_compatible((static_cast<UArrType*>(t1))->component, t);
    } else {
        // t1->tag must be rec
        for (int i = 0; i < (static_cast<URecType*>(t1))->card; i++) {
            if (!deep_compatible((static_cast<URecType*>(t1))->component[i], t)) {
                return false;
            }
        }
        return true;
    }
}

UValueList* UVarTerm::evaluate() {
    UValueList* vl = new UValueList;
    vl->next = NULL;
    vl->val = v->get();
//    char* c = vl->val->text(); // unused!?
    return vl;
}

UValueList* UOpTerm::evaluate() {
    UValueList* vl = NULL;
    UValueList** last;
    unsigned int i;

    UValueList** subresult = new UValueList * [arity + 1] ;
    UValueList** subindex = new UValueList * [arity + 1 ] ;

    for (unsigned int i = 0; i < arity; i++) {
        subresult[i] = sub[i]->evaluate();
        if (!(subresult[i])) {
            return NULL;
        }
        subindex[i] = subresult[i];
    }

    // Jetzt f fuer jede Kombination von Teilresultaten aufrufen
    while (1) {
        // 1. Funktionswert fuer derzeitige Argumentkombination
        f->enter();
        for (i = 0; i < arity; i++) {
            (f->formalpar)[i]->assign((subindex[i])->val);
        }
        try {
            f->body->execute();
        } catch (ExitException) {}
        for (last = &vl; * last; last = & ((* last)->next)) {
            ;
        }
        *last = f->result;
//        for (UValueList* vc = * last; vc; vc = vc->next) {
//            char* bla = vc->val->text(); // unused!?
//        }
        f->leave();

        // 2. Neue Argumentkombination
        if (!arity) {
            return vl;
        }
        i = arity - 1;
        while (1) {
            if (subindex[i]->next) {
                subindex[i] = subindex[i]->next;
                break;
            } else {
                subindex[i] = subresult[i];
                if (i > 0) {
                    i--;
                    continue;
                } else {
                    // keine weiteren Argumenttupel
                    delete [] subindex;
                    for (i = 0; i < arity; i++) {
                        UValueList* tmp;

                        while (subresult[i]) {
                            tmp = subresult[i];
                            subresult[i] = subresult[i]->next;
                            delete tmp;
                        }
                    }
                    delete [] subresult;
//                    for (UValueList* vc = vl; vc; vc = vc->next) {
//                        char* bla = vc->val->text(); // unused!?
//                    }
                    return vl;
                }
            }
        }
    }
}

fmode::fmode() : v(NULL), t(NULL), next(NULL) {}

fmode::fmode(VaSymbol* _v, UExpression* _t, fmode* _next) : v(_v), t(_t), next(_next) {}
