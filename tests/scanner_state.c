
#include <stdio.h>
#include <stdlib.h>

static inline void push_scanner_state(int state);
static inline int peek_scanner_state(void);
static inline void pop_scanner_state(void);

int indent = 0;

void print_indent(void) {
	fprintf(stderr, "%*s", indent*2, "");
}

#define ENTER do{ \
		print_indent(); \
		printf("enter: %s\n", __func__); \
		indent++; \
	}while(0)

#define RETURN(...) do{ \
		indent--; \
		print_indent(); \
		printf("return: %s\n", __func__); \
		return __VA_ARGS__; \
	}while(0)

#define TRACE(...) do{ \
		print_indent(); \
		printf(__VA_ARGS__); \
		printf("\n"); \
	}while(0)
	
#define BEGIN(x) do{ \
		print_indent(); \
		printf("%s BEGIN(%d): %d\n", __func__, scanner_stack[scanner_tos-1],scanner_tos); \
	}while(0)
	
#define FATAL(...) do{ \
		printf("fatal: "); \
		printf(__VA_ARGS__); \
		exit(1); \
	}while(0)
	
#define SCANNER_STACK_SIZE 1024


static int scanner_stack[SCANNER_STACK_SIZE];
static int scanner_tos = 0;
static inline void push_scanner_state(int state) {
    ENTER;
    TRACE("push state: %d", state);
    if(scanner_tos > SCANNER_STACK_SIZE)
        FATAL("scanner stack over run");
    else {
        scanner_stack[scanner_tos] = state;
        scanner_tos++;
        BEGIN(state);
    }
    RETURN();
}

static inline int peek_scanner_state(void) {
    ENTER;
    if(scanner_tos >= 0 && scanner_tos < SCANNER_STACK_SIZE) {
        int state = scanner_stack[scanner_tos-1];
        TRACE("peek state: %d", state);
        RETURN(state);
    }
    else
        FATAL("scanner stack under run");
    RETURN(-1);
}

static inline void pop_scanner_state(void) {
    ENTER;
    TRACE("old state: %d", peek_scanner_state());
    if(scanner_tos == 0)
        FATAL("scanner stack under run");
    else {
        scanner_tos--;
        int state = scanner_stack[scanner_tos-1];
        TRACE("pop state: %d", state);
        BEGIN(state);
    }
    RETURN();
}

int main(void) {

	ENTER;
	push_scanner_state(0);
	push_scanner_state(10);
	push_scanner_state(12);
	pop_scanner_state();
	RETURN(0);
}
