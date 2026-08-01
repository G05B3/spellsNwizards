CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -g

SRC_DIR = src

CARDDB = carddb
VIEWER = card_viewer

COMMON_SRC = $(SRC_DIR)/cJSON.c

CARDDB_SRC = \
	$(SRC_DIR)/create_cards.c \
	$(COMMON_SRC)

VIEWER_SRC = \
	$(filter-out $(SRC_DIR)/create_cards.c,$(wildcard $(SRC_DIR)/*.c))

all: $(CARDDB) $(VIEWER)

$(CARDDB): $(CARDDB_SRC)
	$(CC) $(CFLAGS) $^ -o $@

$(VIEWER): $(VIEWER_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(CARDDB) $(VIEWER)

run: $(VIEWER)
	./$(VIEWER)

.PHONY: all clean run