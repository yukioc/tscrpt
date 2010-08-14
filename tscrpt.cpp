/*!
 * \file
 * \brief tiny script language
 * \mainpage
 * \include README
 */
//#include <map>
#include <cstdio> //printf
#include <cstdlib> //strtol
#include <cctype> //isspace,isdigit,isalpha,isalnum
#include <cstring> //strndup,strcmp
#include <cassert> //assert
#include <stdexcept> //logic_error
#include <sstream> //ostringstream

#include "tscrpt.h" //string,list
enum vlevel_t {VERR=3,VWARN=4,VNOTICE=5,VINFO=6,VLOG=7,VDBG=8,VTRACE=9};
vlevel_t verbose_lvl=VNOTICE;
#define verbose(err,...) if(verbose_lvl>=(err)){printf(__VA_ARGS__);}
inline std::string itos(int i){
	std::ostringstream os;
	os<<i;
	return os.str();
}

//------------------------------------------------------------------------------
// obj_t
int obj_t::value(){
	switch(type){
		case 'N': break;
		case 'S': num=(int)strtol(_str.c_str(),NULL,0); break;
		default: assert(0);
	}
	return num;
}
std::string& obj_t::str(){
	switch(type){
		case 'N': _str=itos(num); break;
		case 'S': break;
		case '$': _str='$'; break;
		default: assert(0); break;
	}
	return _str;
}

//------------------------------------------------------------------------------
// tscrpt_t

tscrpt_t::tscrpt_t() : objs(0) {}
tscrpt_t::~tscrpt_t(){ clear(); }

obj_t* tscrpt_t::eval(const char* s) {
	std::list<obj_t*> objs;
	scan(&objs,s);
	obj_t* o=parse(&objs);
	return o;
}

/*!
 * \brief token scanner.
 * \param[out] olist the list of detected token objects
 * \param[in] s the string to analyze.
 */
void tscrpt_t::scan(std::list<obj_t*>* olist, const char* s)
{
	int i;
	const char *e;
	verbose(VTRACE,"scan: text=%s\n", s);
	assert(olist!=0&&s!=0);
	while(*s!=0){
		for(;isspace(*s);s++) if(*s=='\0') return; //skip space
		if(isdigit(*s)){ //number, [0-9]+ 0[0-7]+ 0x[0-9a-fA-F]+
			olist->push_back(new obj_t('N',strtol(s,(char**)&e,0)));
			s=e;
			if (isalpha(*s)||*s=='_') verbose(VERR,"Error: wrong trailing character\n");
		}else if(isalpha(*s)||*s=='_'){ //symbol, [_a-zA-Z][_a-zA-Z0-9]+
			for(i=0,e=s;isalnum(*e)||*e=='_';i++,e++);
			olist->push_back(new obj_t('I',s,i));
			s=e;
		}else if(strcmp(s,"//")==0){ //line comment
			verbose(VTRACE,"scan: comment\n");
			while(*++s!='\0');
		}else if(*s=='+'||*s=='-'||*s=='*'||*s=='/'||*s=='%'){ //operator
			olist->push_back(new obj_t(*s));
			++s;
		}else if(*s=='\"'){ //string
			for(i=0,e=s+1;*e!='\"'||*(e-1)=='\\';i++,e++)
				if(*e=='\0') throw std::logic_error("scan: string terminator(\") is not found");
			const char* t=estrndup(s+1,i);
			olist->push_back(new obj_t('S',t));
			delete t;
			s=e+1;
		}else{
			verbose(VERR,"scan: Error: unknown leading character - %c\n", *s);
			++s;
		}
	}
	olist->push_back(new obj_t('$'));
}

obj_t* tscrpt_t::parse(std::list<obj_t*>* olist) {
	if(olist==0) throw std::logic_error("parse: received obj is null");
	assert(objs==0);
	objs=olist;
	obj_t* ret=0;
	while(more() && !is('$')){
		delete ret;
		ret=expr();
	}
	assert(objs->empty());
	objs=0;
	return ret;
}

void tscrpt_t::clear() {
	if(objs!=0) while(!objs->empty()) delete obj();
	objs=0;
}

obj_t* tscrpt_t::obj() {
	if (objs->empty()) throw std::runtime_error( "obj: not found next token" );
	obj_t* o(objs->front());
	objs->pop_front();
	return o;
}
bool tscrpt_t::is(char c,bool eat) {
	if (objs->empty()) throw std::runtime_error( "is: not found next token" );
	obj_t* o=objs->front();
	bool chk(c==o->type);
	if (eat && chk){
		objs->pop_front();
		delete o;
	}
	return chk;
}
bool tscrpt_t::eq(char c) { return is(c,false); }
bool tscrpt_t::be(char c) {
	if(!is(c)) throw std::runtime_error( "be: not found next token" );
	return true;
}
bool tscrpt_t::more() { return !objs->empty(); }

obj_t* tscrpt_t::expr() {
	obj_t *o=term();
	while(more()){ // add,sub
		obj_t *l(0),*r(0);
		if     (is('+')){ o=add(l=o, r=term()); }
		else if(is('-')){ o=sub(l=o, r=term()); }
		else break;
		delete l;
		delete r;
	}
	return o;
}

obj_t* tscrpt_t::term() {
	obj_t *o(factor());
	while(more()){ // mul,div
		obj_t *l(0),*r(0);
		try {
			if     (is('*')){ o=mul(l=o, r=factor()); }
			else if(is('/')){ o=div(l=o, r=factor()); }
			else if(is('%')){ o=rem(l=o, r=factor()); }
			else break;
		}catch(std::runtime_error& e){
			delete l;
			delete r;
			clear();
			throw;
		}
		delete l;
		delete r;
	}
	return o;
}

obj_t* tscrpt_t::factor() {
	obj_t *o(0);
	bool minus=false;
	if     (is('+')){ } // unary-operator
	else if(is('-')){ minus=true; }
//	else if(is('!')){ assert(0); }
//	else if(is('~')){ assert(0); }
//	else if(is('*')){ assert(0); }
//	else if(is('&')){ assert(0); }
	if      (is('(')){ o=expr(); be(')'); }
	else if (eq('I')){ assert(0); }
	else if (eq('N')||eq('S')){ o=obj(); }
	else throw std::runtime_error("factor: syntax error");
	if (minus) {
		obj_t l('N',-1), *r;
		o=mul(&l,r=o);
		delete r;
	}
	return o;
}

obj_t* tscrpt_t::add(obj_t* l, obj_t* r) {
	obj_t* o(0);
	verbose(VTRACE,"parse: %s+%s\n",l->str().c_str(), r->str().c_str());
	if(l->type=='N'){
		o = new obj_t('N', l->value() + r->value());
	}else if(l->type=='S'){
		o = new obj_t('S', l->str() + r->str());
	}else assert(0);
	return o;
}
obj_t* tscrpt_t::sub(obj_t* l, obj_t* r){
	obj_t* o(0);
	verbose(VTRACE,"parse: %s-%s\n",l->str().c_str(), r->str().c_str());
	if(l->type=='N'||l->type=='S'){
		o = new obj_t('N', l->value() - r->value());
	}else assert(0);
	return o;
}
obj_t* tscrpt_t::mul(obj_t* l, obj_t* r){
	obj_t* o(0);
	verbose(VTRACE,"parse: %s*%s\n",l->str().c_str(), r->str().c_str());
	if(l->type=='N'){
		o = new obj_t('N', l->value() * r->value());
	}else if(l->type=='S'){
		o = new obj_t('S', "");
		std::string& s=o->str();
		for(int i=r->value();i>0;--i) s.append(l->str());
	}else assert(0);
	return o;
}
obj_t* tscrpt_t::div(obj_t* l, obj_t* r){
	obj_t* o(0);
	verbose(VTRACE,"parse: %s/%s\n",l->str().c_str(), r->str().c_str());
	if(l->type=='N'||l->type=='S'){
		if(r->value()==0) throw std::runtime_error("div: divide by zero");
		o = new obj_t('N', l->value() / r->value());
	}else assert(0);
	return o;
}
obj_t* tscrpt_t::rem(obj_t* l, obj_t* r){
	obj_t* o(0);
	verbose(VTRACE,"parse: %s%%%s\n",l->str().c_str(), r->str().c_str());
	if(l->type=='N'||l->type=='S'){
		if(r->value()==0) throw std::runtime_error("div: divide by zero");
		o = new obj_t('N', l->value() % r->value());
	}else assert(0);
	return o;
}

//------------------------------------------------------------------------------
// estrndup

/*!
 * \brief duplicate and un-escape a string.
 * \param[in] s an escaped string.
 * \param[in] n the size of an escaped string.
 * \retval a pointer to the duplicated string.
 * \retval NULL return the value if s is NULL.
 * \sa strndup
 * \bug read data too much when parameter \\xnn or \\nnn data
 * is stepped over the string length.
 */
char* estrndup(const char*s, size_t n){
	if (s==NULL)return NULL;
	char *ret(new char[n+1]),*t(ret),*p;
	const char *e(&s[n]);
	for(;*s!='\0'&&s!=e&&n>0;--n,++s,++t){
//		printf("estrndup: s=%c\n",*s);
		if(*s=='\\'){
			++s;--n;
			switch(*s){
				case 'a':*t='\a';break;
				case 'b':*t='\b';break;
				case 'f':*t='\f';break;
				case 'n':*t='\n';break;
				case 'r':*t='\r';break;
				case 't':*t='\t';break;
				case 'v':*t='\v';break;
				case '?':*t='\?';break;
				case '\\':*t='\\';break;
				case '\'':*t='\'';break;
				case '\"':*t='\"';break;
				case 'x':*t=(char)strtol(s+1,&p,16);s=p-1;break;
				default:
					if(*s>='0'&&*s<='7'){ *t=strtol(s,&p,8);s=p-1;break; }
					else {
						delete ret; 
						throw std::logic_error("estrndup: contained wrong escaped character");
					}
			}
		}else if(*s=='"'){
			delete ret;
			throw std::logic_error("estrndup: contained un-escaped '\"'");
		}else{
			*t=*s;
		}
	}
	*t='\0';
	return ret;
}

