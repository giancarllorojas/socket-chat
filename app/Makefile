# October 2006
# Prof.: Silvana Rossetto
#------------------------------------------------------------

VERSION = 1.0

P_SERVER = peer_passive.c
P_CLIENT = peer_active.c
O_CLIENT = client.c
O_SERVER = server.c
SRV = passive
CLI = active
ONLY_CLI = client
ONLY_SRV = server

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
all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(CLI) $(BIN)/$(ONLY_CLI) $(BIN)/$(ONLY_SRV)

$(OBJ)/mysocket.o: $(SRC)/mysocket.c $(INC)/mysocket.h
	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.c

$(BIN)/$(SRV): $(OBJ)/mysocket.o $(P_SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(P_SERVER) $(LFLAGS) 

$(BIN)/$(CLI): $(OBJ)/mysocket.o $(P_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(P_CLIENT)

$(BIN)/$(ONLY_CLI): $(OBJ)/mysocket.o $(O_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(O_CLIENT)

$(BIN)/$(ONLY_SRV): $(OBJ)/mysocket.o $(O_SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(O_SERVER) $(LFLAGS) 
# version
version: 
	@echo $(VERSION) "(Draft Version!)" 
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
