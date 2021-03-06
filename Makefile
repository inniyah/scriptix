PROGRAM=scriptix
LIBNAME=scriptix
LIBRARY=lib$(LIBNAME)

CC= gcc
CXX= g++
AR= ar
RM= rm -f

PKGCONFIG= pkg-config
PACKAGES= 

all: static dynamic program test

static: $(LIBRARY).a
dynamic: $(LIBRARY).so
program: $(PROGRAM)

MAJOR=0
MINOR=0

LIB_SRC = \
	src/array.cpp \
	src/eval.cpp \
	src/grammar.cpp \
	src/name.cpp \
	src/system.cpp \
	src/compiler.cpp \
	src/function.cpp \
	src/iter.cpp \
	src/number.cpp \
	src/string.cpp \
	src/type.cpp \
	src/error.cpp \
	src/global.cpp \
	src/optimize.cpp \
	src/struct.cpp \
	src/value.cpp \
	src/parser.cpp \
	src/lexer.cpp

PRG_SRC = \
	program/scriptix.cpp

SHARED_OBJS = $(LIB_SRC:.cpp=.shared.o)
STATIC_OBJS = $(LIB_SRC:.cpp=.static.o)
PROGRAM_OBJS = $(PRG_SRC:.cpp=.o)

CFLAGS= -O2 -g -Wall -std=c++17 \
	-Iinclude \
	-fstack-protector-strong \
	-Wall \
	-Wformat \
	-Werror=format-security \
	-Wdate-time \
	-D_FORTIFY_SOURCE=2

CFLAGS+= $(shell $(PKGCONFIG) --cflags $(PACKAGES))

STATIC_CFLAGS= $(CFLAGS)
SHARED_CFLAGS= $(CFLAGS) -fPIC

LDFLAGS= \
	-Wl,--as-needed \
	-Wl,--no-undefined \
	-Wl,--no-allow-shlib-undefined

LIBS= -lgc -lgccpp

LIBS+= $(shell $(PKGCONFIG) --libs $(PACKAGES) 2>/dev/null)

ARFLAGS= cr

$(PROGRAM): $(PROGRAM_OBJS) $(LIBRARY).a $(LIBRARY).so
	g++ $(LDFLAGS) $(PROGRAM_OBJS) -l$(LIBNAME) -L. $(LIBS) -o $@

$(LIBRARY).so.$(MAJOR).$(MINOR): $(SHARED_OBJS)
	$(CXX) $(LDFLAGS) -shared \
		-Wl,-soname,$(LIBRARY).so.$(MAJOR) \
		-o $(LIBRARY).so.$(MAJOR).$(MINOR) \
		$+ -o $@ $(LIBS)

$(LIBRARY).so: $(LIBRARY).so.$(MAJOR).$(MINOR)
	rm -f $@.$(MAJOR)
	ln -s $@.$(MAJOR).$(MINOR) $@.$(MAJOR)
	rm -f $@
	ln -s $@.$(MAJOR) $@

$(LIBRARY).a: $(STATIC_OBJS)
	$(AR) $(ARFLAGS) $@ $+

%.shared.o: %.cpp
	g++ -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: %.c
	gcc -o $@ -c $+ $(SHARED_CFLAGS)

%.so : %.o
	g++ $(LDFLAGS) -shared $^ -o $@

%.static.o: %.cpp
	g++ -o $@ -c $+ $(STATIC_CFLAGS)

%.static.o: %.c
	gcc -o $@ -c $+ $(STATIC_CFLAGS)

%.o: %.cpp
	g++ -o $@ -c $+ $(STATIC_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(STATIC_CFLAGS)

%.cpp %.h: %.ll
	flex --outfile=$@ --header-file=$(@:.cpp=.h) $<

%.cpp %.h: %.yy
	bison --output=$@ --defines=$(@:.cpp=.h) $<

clean:
	rm -f $(SHARED_OBJS)
	rm -f $(STATIC_OBJS)
	rm -f $(PROGRAM_OBJS)
	rm -f $(PROGRAM)
	rm -f src/lexer.cpp src/lexer.h
	rm -f src/grammar.cpp src/grammar.h
	rm -f *.so *.so* *.a *~

.depend depend dep: $(LIB_SRC) $(PRG_SRC)
	g++ $(CFLAGS) -MM $(LIB_SRC) $(PRG_SRC) $(INCLUDE) $(PKG_CONFIG_CFLAGS) > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

test: $(PROGRAM)
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/array.sx      >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/big.sx        >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/comments.sx   >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/extend.sx     >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/fact.sx       >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/global.sx     >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/inc.sx        >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/iter.sx       >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/logic.sx 4 3  >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/string.sx     >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/struct.sx     >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/types.sx      >/dev/null
	LD_LIBRARY_PATH="." ./$(PROGRAM) test/loops.sx      >/dev/null

	@echo ; echo Tests OK!

.PHONY: all static dynamic program depend dep clean install
