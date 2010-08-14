# Makefile

TEST:=1

##### FILE
APP_EXE		:=tscrpt
#APP_VER	:=$(shell git describe)
APP_SRC		:=tscrpt.cpp
TEST_SRC	:=t-tscrpt.cpp

APP_O		:=$(APP_SRC:%.cpp=%.o)
SRC_ALL		:=$(APP_SRC)
SRC_ALL		+=$(TEST_SRC)

##### PATH
#vpath %.c t
#vpath %.cpp t
#vpath %.h t

##### COMMAND
CC	:=g++
CXX	:=g++

CPPFLAGS	:=-O3
CPPFLAGS	+=-Wall -Wextra
CPPFLAGS	+=-Wno-unused-parameter
CPPFLAGS	+=-Wformat=2 -Wstrict-aliasing=2 -Wdisabled-optimization -Wfloat-equal -Wpointer-arith -Wcast-align -Wredundant-decls
#CPPFLAGS	+=--param max-inline-insns-single=12000 --param large-function-growth=28000 --param inline-unit-growth=400
#CPPFLAGS	+=-Wbad-function-cast -Wdeclaration-after-statement # for c
#CPPFLAGS	+=-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 # support over 4GB file.
#INCLUDES	:=-Isrc
INCLUDES	:=-I.

##### OPTIONAL
ifdef APP_VER
CPPFLAGS	+=-DVERSION="\"${APP_VER}\""
endif
ifdef TEST
CPPFLAGS	+=-g
CPPFLAGS	+=-DTEST -ftest-coverage -fprofile-arcs 
APP_O		+=$(TEST_SRC:%.cpp=%.o)
endif

##### TARGET
.PHONY: all
all: .depend $(APP_EXE)

.PHONY: test
test: all tp gcov

.PHONY: tp
tp:
	@echo "### test"
	./$(APP_EXE) --test

.PHONY: gcov
gcov:
	@echo "### gcov"
	@gcov $(SRC_ALL) | \
	grep -v creating | \
	perl -pe 's/File .([\S]+).\n/\1 /; s/Lines \w+:(.+)\n/\1/;' | \
	grep -v -E "(^/usr|^t/)" | \
	perl -ne 'printf "%20s %7s %s %3s\n", split;'

.PHONY: depend
depend: .depend

.PHONY: clean
clean:
	@echo "### clean"
	rm -f *.o
	rm -f *.gcno *.gcda *.gcov
	rm -f .depend

.PHONY: ctags
ctags:
	ctags -R --tag-relative=no --fields=+iaS --extra=+q

.PHONY: doxygen
doxygen : ${SRC_ALL} ${HEADERS}
	doxygen 

.depend : $(SRC_ALL)
	@echo "### depend"
	${CXX} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -MM $^ > $@

ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "depend"
ifneq "$(MAKECMDGOALS)" "ctags"
include .depend
endif
endif
endif

$(APP_EXE) : $(APP_O)
	@echo "### $@ ###"
	@${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -o $@ $^

##### SUFFIX

.SUFFIXES: .cpp .c

.c.o:
	@echo "### $@ ###"
	@rm -f $($@:%.o=%.gcno) $($@:%.o=%.gcda)
	@${CXX} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -x c -c -o $@ $<
#	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c -o $@ $<

.cpp.o:
	@echo "### $@ ###"
	@${CXX} ${CFLAGS} ${CXXFLAGS} ${CPPFLAGS} ${INCLUDES} -c -o $@ $<

