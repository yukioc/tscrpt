/*! \file
 *  \brief tscrpt test
 */
// vim:fdm=marker
#include <typeinfo> //typeid
#include <stdexcept>
#include <cstring> //strlen,strcmp
#include <cassert> //assert

#include "tscrpt.h"
#include "munit.h"
int mu_nfail=0;
int mu_ntest=0;
int mu_nassert=0;

void t_estrndup(){/*{{{*/
	bool except;
	char* s;
	//! \test NULL ptr => NULL
	s=estrndup(NULL,1);
	mu_assert(s==NULL);

	//! \test zero => ""
	s=estrndup("hello",0);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==0);
	delete s;

	//! \test hello => hello
	s=estrndup("hello",5);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==5);
	mu_assert(strcmp(s,"hello")==0);
	delete s;

	//! \test hello,n=4 => hell
	s=estrndup("hello",4);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==4);
	mu_assert(strcmp(s,"hell")==0);
	delete s;

	//! \test first\\0second,n=12 => first
	s=estrndup("first\0second",12);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==5);
	mu_assert(strcmp(s,"first")==0);
	delete s;

	//! \test say "hello" => say "hello"
	s=estrndup("say \\\"hello\\\"",13);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==11);
	mu_assert(strcmp(s,"say \"hello\"")==0);
	delete s;

	//! \test esc chars => unescaped
	s=estrndup("\\a,\\b,\\f,\\n,\\r,\\t,\\v,\\?,\\\\,\\',\\\"$",33);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==22);
	mu_assert(strcmp(s,"\a,\b,\f,\n,\r,\t,\v,\?,\\,\',\"$")==0);
	delete s;

	//! \test \\x4Fh! => Oh!
	s=estrndup("\\x4Fh!",6);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==3);
	mu_assert(strcmp(s,"Oh!")==0);
	delete s;

	//! \test \\107ood => Good
	s=estrndup("\\107ood",7);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==4);
	mu_assert(strcmp(s,"Good")==0);
	delete s;

	//! \test FIRST\\0SECOND => FIRST\\0SECOND\\0
	s=estrndup("FIRST\\0SECOND",13);
	mu_assert(s!=NULL);
	mu_assert(strlen(s)==5);
	mu_assert(memcmp(s,"FIRST\0SECOND\0",13)==0);
	delete s;
	s=0;

	//! \test aaa"bbb => exception
	except=false;
	try {
		s=estrndup("aaa\"bbb",7);
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::logic_error));
	}
	assert(except==true);

	//! \test \\z => exception
	except=false;
	try {
		s=estrndup("\\z",2);
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::logic_error));
	}
	assert(except==true);
}/*}}}*/
void t_number() {/*{{{*/
	tscrpt_t sc;
	obj_t*o;
	//! \test decimal: 1 => 1
	o = sc.eval("1");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==1);
	delete o;

	//! \test octal: 012 => 10
	o = sc.eval("012");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==10);
	mu_assert(o->str().compare("10")==0);
	delete o;

	//! \test hexadecimal: 0x14 => 20
	o = sc.eval("0x14");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==20);
	delete o;
}/*}}}*/
void t_string() {/*{{{*/
	tscrpt_t sc;
	obj_t*o;

	//! \test hello => hello
	o = sc.eval("\"hello\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("hello")==0);
	delete o;

	//! \test say \\"hello\\" => say "hello"
	o = sc.eval("\"say \\\"hello\\\"\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("say \"hello\"")==0);
	delete o;

	//! \test \\"\\" => ""
	o = sc.eval("\"\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("")==0);
	delete o;
}/*}}}*/
void t_unaryop(){/*{{{*/
	tscrpt_t sc;
	obj_t*o;
	//! \test + 2 => 2
	o = sc.eval("+ 2");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==2);
	delete o;

	//! \test +2 => 2
	o = sc.eval("+2");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==2);
	delete o;

	//! \test - 2 => -2
	o = sc.eval("-2");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-2);
	delete o;
}/*}}}*/
void t_mul(){ /*{{{*/
	tscrpt_t sc;
	obj_t*o;

	//! \test 2 * 3 => 6
	o = sc.eval("2 * 3");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==6);
	delete o;

	//! \test -4 * 5 => -20
	o = sc.eval("-4 * 5");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-20);
	delete o;

	//! \test 6 * -7 => -42
	o = sc.eval("6 * -7");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-42);
	delete o;

	//! \test -8 * -9 => 72
	o = sc.eval("-8 * -9");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==72);
	delete o;

	//! \test 0 * -10 => 0
	o = sc.eval("0 * -10");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==0);
	delete o;

	//! \test "+-" * 3 => "+-+-+-"
	o = sc.eval("\"+-\" * 3");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("+-+-+-")==0);
	delete o;

	//! \test 10 * "mmm" => 0
	o = sc.eval("10 * \"mmm\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==0);
	delete o;
}/*}}}*/
void t_div(){ /*{{{*/
	bool except;
	tscrpt_t sc;
	obj_t*o;

	//! \test 12 / 4 => 3
	o = sc.eval("12 / 4");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==3);
	delete o;

	//! \test 13 / 5 => 2
	o = sc.eval("13 / 5");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==2);
	delete o;
	o=0;

	//! \test 14 / 0 => runtime_error
	except=false;
	try {
		o=sc.eval("14 / 0");
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::runtime_error));
		sc.clear();
	}
	mu_assert(except==true);

	//! \test "ddd" / 6 => 0
	o = sc.eval("\"ddd\" / 6");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==0);
	delete o;

	//! \test 11 / "ddd" => runtime_error
	except=false;
	try {
		o = sc.eval("11 / \"ddd\"");
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::runtime_error));
	}
	mu_assert(except==true);
}/*}}}*/
void t_rem(){ /*{{{*/
	bool except;
	tscrpt_t sc;
	obj_t*o;

	//! \test 15 % 5 => 0
	o = sc.eval("15 % 5");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==0);
	delete o;

	//! \test 16 % 6 => 4
	o = sc.eval("16 % 6");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==4);
	delete o;

	//! \test -17 % 7 => -3
	o = sc.eval("-17 % 7");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-3);
	delete o;
	o=0;

	//! \test 18 % -10 => 8
	o = sc.eval("18 % -10");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==8);
	delete o;
	o=0;

	//! \test 9 % 0 => runtime_error
	except=false;
	try {
		o=sc.eval("9 % 0");
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::runtime_error));
	}
	mu_assert(except==true);

	//! \test "rrr" % 7 => exception
	o = sc.eval("\"rrr\" % 7");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==0);

	//! \test 12 % "rrr" => exception
	except=false;
	try {
		o = sc.eval("12 % \"rrr\"");
	}catch(std::exception& e){
		except=true;
		mu_assert(typeid(e)==typeid(std::runtime_error));
	}
	mu_assert(except==true);
}/*}}}*/
void t_add(){/*{{{*/
	tscrpt_t sc;
	obj_t*o;

	//! \test 2 + 3 => 5
	o = sc.eval("2 + 3");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==5);
	delete o;

	//! \test 4+6 => 10
	o = sc.eval("4+6");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==10);
	delete o;

	//! \test "hello," + "world" => "hello,world"
	o = sc.eval("\"hello,\" + \"world\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("hello,world")==0);
	delete o;

	//! \test "aaa" + "" => "aaa"
	o = sc.eval("\"aaa\" + \"\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("aaa")==0);
	delete o;

	//! \test "" + "bbb" => "bbb"
	o = sc.eval("\"\" + \"bbb\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("bbb")==0);
	delete o;

	//! \test "ccc" + 4 => "ccc4"
	o = sc.eval("\"ccc\" + 4");
	mu_assert(o!=NULL);
	mu_assert(o->type=='S');
	mu_assert(o->str().compare("ccc4")==0);
	delete o;

	//! \test 8 + "aaa" => 8
	o = sc.eval("8 + \"ddd\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==8);
	delete o;
}/*}}}*/
void t_sub(){/*{{{*/
	tscrpt_t sc;
	obj_t*o;

	//! \test 7-5 => 2
	o = sc.eval("7-5");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==2);
	delete o;

	//! \test 8-17 => -9
	o = sc.eval("7-0x10");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-9);
	delete o;

	//!\test 2+3-4+5+-6+7-+8 => -1
	o = sc.eval("2+3-4+5+-6+7-+8");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-1);
	delete o;

	//! \test "5" - 7 => "-2"
	o = sc.eval("\"5\" - 7");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-2);
	delete o;

	//! \test "ddd" - 8 => "-8"
	o = sc.eval("\"ddd\" - 8");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==-8);
	delete o;

	//! \test "0xf" - "3" => "12"
	o = sc.eval("\"0xf\" - \"3\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==12);
	delete o;

	//! \test 9 - "sss" => 9
	o = sc.eval("9 - \"sss\"");
	mu_assert(o!=NULL);
	mu_assert(o->type=='N');
	mu_assert(o->value()==9);
	delete o;
}/*}}}*/

int main(int argc, char* argv[])
{
//	symbol_table.insert(symbol(":"));

	mu_run_test(t_estrndup);
	mu_run_test(t_number);
	mu_run_test(t_string);
	mu_run_test(t_unaryop);
	mu_run_test(t_mul);
	mu_run_test(t_div);
	mu_run_test(t_rem);
	mu_run_test(t_add);
	mu_run_test(t_sub);
	mu_show_failures();
	return 0;
}

