/*! \file
 *  \brief tscrpt header file.
 */
#ifndef __tscrpt_h__
#define __tscrpt_h___
#include <string>
#include <list>

class obj_t {
	public:
		/*! \brief object type.
		 * - 'N' : number
		 * - 'I' : identifier
		 * - 'S' : string
		 * - 'V' : variable
		 * - 'F' : function
		 * - 'A' : array
		 * - 'K' : keyword
		 * - '+' '-' '*' '/' '%' '(' ')' : operator
		 * - '$' : terminator
		 */
		char type;
	private:
		std::string _str;
		int num;
	public:
		obj_t(char type):type(type),_str(),num(0) {}
		obj_t(char type,int num):type(type),_str(),num(num){}
		obj_t(char type,const char* s):type(type),_str(s),num(0) {}
		obj_t(char type,const char* s,int len):type(type),_str(s,len),num(0) {}
		obj_t(char type,const std::string& s):type(type),_str(s),num(0) {}
		~obj_t(){}
		int value();
		std::string& str();
};

class tscrpt_t {
	private:
		std::list<obj_t*>* objs;

	public:
		tscrpt_t();
		~tscrpt_t();
		obj_t* eval(const char* s);
		void scan(std::list<obj_t*>* olist,const char* s); //!< \brief scan script string
		obj_t* parse(std::list<obj_t*>* olist); //!< \brief parse object list
		void clear(); //!< \brief clear internal obj list(token list)

	private:
		//token analyzing
		obj_t* obj(); //!< \brief get token.
		bool is(char c,bool eat=true); //!< \brief eat if next obj type is.
		bool eq(char c); //!< \brief check preceding obj type.
		bool be(char c); //!< \brief must be.
		bool more(); //!< \brief is more obj?

		//syntax analyzing
		obj_t* expr();
		obj_t* term();
		obj_t* factor();

	public:
		//object operation
		obj_t* add(obj_t* l, obj_t* r); //!< \brief number+number or string+string
		obj_t* sub(obj_t* l, obj_t* r); //!< \brief number-number
		obj_t* mul(obj_t* l, obj_t* r); //!< \brief number*number or string*number
		obj_t* div(obj_t* l, obj_t* r); //!< \brief number/number
		obj_t* rem(obj_t* l, obj_t* r); //!< \brief number%number
};

char* estrndup(const char*s, size_t n); //!< \brief deescape and duplicate string.

#endif /*__tscrpt_h__*/

