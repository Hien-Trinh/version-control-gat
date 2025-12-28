CC = gcc

OPENSSL_DIR = /opt/homebrew/opt/openssl@3

CFLAGS = -Wall -Wextra -Iinclude -I$(OPENSSL_DIR)/include
LDFLAGS = -L$(OPENSSL_DIR)/lib

LIBS = -lz -lcrypto

SRC = src/main.c src/init.c src/object.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = gat

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJ) $(TARGET)