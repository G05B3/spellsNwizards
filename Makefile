CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -g

SRC_DIR = src

CARDDB = carddb
VIEWER = card_viewer
DECK_BUILDER = deck_builder

COMMON_SRC = \
    $(SRC_DIR)/cJSON.c

CARDDB_SRC = \
    $(SRC_DIR)/create_cards.c \
    $(COMMON_SRC)

VIEWER_SRC = \
    $(filter-out \
        $(SRC_DIR)/create_cards.c \
        $(SRC_DIR)/deck_builder.c, \
        $(wildcard $(SRC_DIR)/*.c))

DECK_BUILDER_SRC = \
    $(SRC_DIR)/deck_builder.c \
    $(SRC_DIR)/cards.c \
    $(SRC_DIR)/access.c \
    $(COMMON_SRC)


all: $(CARDDB) $(VIEWER) $(DECK_BUILDER)


$(CARDDB): $(CARDDB_SRC)
	$(CC) $(CFLAGS) $^ -o $@


$(VIEWER): $(VIEWER_SRC)
	$(CC) $(CFLAGS) $^ -o $@


$(DECK_BUILDER): $(DECK_BUILDER_SRC)
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm -f $(CARDDB) $(VIEWER) $(DECK_BUILDER)


run: $(VIEWER)
	./$(VIEWER)


run_deck_builder: $(DECK_BUILDER)
	./$(DECK_BUILDER)


.PHONY: all clean run run_deck_builder