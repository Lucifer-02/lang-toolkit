CC = gcc 
WARNING += -Wall -Wextra 
# WARNING += -Wcast-qual -Wcast-align -Wstrict-aliasing -Wpointer-arith -Winit-self -Wshadow -Wswitch-enum -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wfloat-equal -Wundef -Wvla -Wc++-compat
TEXT = "VBA, or Visual Basic for Applications in short, is a Microsoft computer programming language which is used to facilitate automation of repetitive Microsoft suite data-processing functions, like generating custom graphs and reports. In this instance, VBA can also be used to automate the process of slides creation in PowerPoint using ChatGPT, an AI that can understand and process both human language and codes."
# TEXT = "hi"


LINKLIBS +=	-lvlc -ljansson -lcurl -lpthread
INCLUDED +=	./lib/tts.c ./lib/trans.c ./lib/common.c

SOURCE = main
SOURCE_CODE = $(SOURCE).c
OUTPFILE += $(SOURCE)

clean:
	rm main

debug:
	$(CC) -g $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) $(WARNING)

release:
	$(CC) -O3 $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) -DNDEBUG

run: release
	./main $(TEXT)
