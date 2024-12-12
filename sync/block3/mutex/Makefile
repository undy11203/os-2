# EXECUTABLES=main
# SOURCES=./

# TYPE=.c
# CC=gcc
# RM=rm
# CFLAGS=-Wall
# LIBS=linkedlist/linkedlist.c mutex/mutex.c
# LDFLAGS=-o $@

# all: $(EXECUTABLES)

# $(EXECUTABLES): %: $(SOURCES)%$(TYPE)
# 	$(CC) $(CFLAGS) $(LDFLAGS) $< ${LIBS}

# run: $(EXECUTABLES)
# 	./${EXECUTABLES}

# clean:
# 	${RM} -f *.o $(EXECUTABLES)

TARGET_1 = linkedlist1
SRCS_1 = mutex/mutex.c linkedlist/linkedlist.c main.c

TARGET_2 = queue1
SRCS_2 = mutex/mutex.c queue/queue.c main1.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2}

${TARGET_1}: mutex/mutex.h linkedlist/linkedlist.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: mutex/mutex.h queue/queue.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2}
