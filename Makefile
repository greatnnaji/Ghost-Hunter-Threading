OPT = -Wall -Wextra -g
TARGET = main.o ghost.o room.o hunter.o evidence.o logger.o utils.o house.o

link:	${TARGET}
		gcc ${OPT} -o a5 ${TARGET}
	
main.o:	main.c defs.h
		gcc ${OPT} -c main.c

room.o:	room.c defs.h
		gcc ${OPT} -c room.c

ghost.o:	ghost.c defs.h
			gcc ${OPT} -c ghost.c

house.o:	house.c defs.h
			gcc ${OPT} -c house.c

hunter.o:	hunter.c defs.h
			gcc ${OPT} -c hunter.c

evidence.o:	evidence.c defs.h
			gcc ${OPT} -c evidence.c

logger.o:	logger.c defs.h
			gcc ${OPT} -c logger.c

utils.o:	utils.c defs.h
			gcc ${OPT} -c utils.c

clean: 
		rm -f a5 ${TARGET}