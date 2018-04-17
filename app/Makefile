# October 2006
# Prof.: Silvana Rossetto
#------------------------------------------------------------

VERSION = 1.0

SERVER = peer_passive.c
CLIENT = peer_active.c
O_CLIENT = client.c
SRV = srv
CLI = cli
ONLY_CLI = o_cli

#-------------------------------------------------------------
# the rest of the file should not change
INC = ../include
SRC = ../src
OBJ = ../obj
BIN = ../bin

CC = gcc -Wall
CFLAGS = -I$(INC) 
LFLAGS = -lpthread 
RM = rm

# make all
all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(CLI) $(BIN)/$(ONLY_CLI)

$(OBJ)/mysocket.o: $(SRC)/mysocket.c $(INC)/mysocket.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.c

$(BIN)/$(SRV): $(OBJ)/mysocket.o $(SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SERVER) $(LFLAGS) 

$(BIN)/$(CLI): $(OBJ)/mysocket.o $(CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(CLIENT)

$(BIN)/$(ONLY_CLI): $(OBJ)/mysocket.o $(O_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(O_CLIENT)
# version
version: 
	@echo $(VERSION) "(Draft Version!)" 
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
