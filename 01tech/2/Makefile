INPUT_NUMBER_EXPRESSION:=$(shell  \
	sed -nE \
	-e '/^.*\#/d' \
	-e '/^$$/d' \
	-e 's/^.+=(.+)$$/\1/g' \
	-e '/[0-9]+/!d' \
	-e 's/(.*)/+ \1/' \
	-e '0,/ / s/\+//' \
	-e '/[0-9]+/p' compile.conf ;\
	)

INPUT_STRINGS:=$(shell  \
	sed -nE \
	-e '/^.*\#/d' \
	-e '/^$$/d' \
	-e 's/^.+=(.+)$$/\1/g' \
	-e '/[0-9]+/d' \
	-e 's/^/\\n/g' \
	-e '0,/\\n/ s/\\n//' \
	-e '/.*/p' compile.conf ;\
	)

.PHONY : all build clean

all : build

build : program

clean :
	@${RM} solution.o
	@${RM} program.exe

solution.o : solution.c compile.conf
	@${CC} -fPIC -c \
	-D STRINGS='"$(INPUT_STRINGS)"' \
	-D SUM=$(shell echo "$(INPUT_NUMBER_EXPRESSION)" | bc -l) \
	-o solution.o solution.c

program : solution.o
	@${CC} -o program.exe solution.o
