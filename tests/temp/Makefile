
TARGET	=	t
OBJS	=	parser.o \
		scanner.o \
		_string.o

%.o:%.c
	gcc -Wall -g -c -o $@ $<

all: $(OBJS)
	gcc -g -o $(TARGET) $^

scanner.c scanner.h: scanner.l
	flex --header-file=scanner.h -io scanner.c $<

parser.c parser.h: parser.y
	bison -dtv -o parser.c $<

clean:
	$(RM) $(TARGET) scanner.c scanner.h parser.c parser.h $(OBJS) *.output
