CC=g++
TARGET:=test
 
all:${TARGET}
 
C_SRCS=$(wildcard *.cpp)
C_OBJS=${patsubst %.cpp, %.o, $(C_SRCS)}
C_Link= -lpq -lpqxx

all:${TARGET}
 
${TARGET}:${C_OBJS}
	${CC} -o $@ $^ ${C_Link}
%.o:%.cpp
	${CC} -c -o $@ $<
.PHONY:clean
clean:
	rm -rf ${TARGET} *.o