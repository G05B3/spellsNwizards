CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -g

SRC_DIR = src

CARDDB = carddb
VIEWER = card_viewer
DECK_BUILDER = deck_builder
BOARD_VIEWER = board_viewer

COMMON_SRC = \
    $(SRC_DIR)/cJSON.c

CARDDB_SRC = \
    $(SRC_DIR)/create_cards.c \
    $(COMMON_SRC)

VIEWER_SRC = \
    $(filter-out \
        $(SRC_DIR)/create_cards.c \
	$(SRC_DIR)/board_viewer.c \
        $(SRC_DIR)/deck_builder.c, \
        $(wildcard $(SRC_DIR)/*.c))

DECK_BUILDER_SRC = \
    $(SRC_DIR)/deck_builder.c \
    $(SRC_DIR)/cards.c \
    $(SRC_DIR)/access.c \
    $(COMMON_SRC)

BOARD_VIEWER_SRC = \
    $(filter-out \
	$(SRC_DIR)/create_cards.c \
	$(SRC_DIR)/card_viewer.c \
	$(SRC_DIR)/deck_builder.c, \
	$(wildcard $(SRC_DIR)/*.c))
	
all: $(CARDDB) $(VIEWER) $(DECK_BUILDER) $(BOARD_VIEWER)


$(CARDDB): $(CARDDB_SRC)
	$(CC) $(CFLAGS) $^ -o $@


$(VIEWER): $(VIEWER_SRC)
	$(CC) $(CFLAGS) $^ -o $@


$(DECK_BUILDER): $(DECK_BUILDER_SRC)
	$(CC) $(CFLAGS) $^ -o $@

$(BOARD_VIEWER): $(BOARD_VIEWER_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(CARDDB) $(VIEWER) $(DECK_BUILDER) $(BOARD_VIEWER)


run: $(VIEWER)
	./$(VIEWER)


run_deck_builder: $(DECK_BUILDER)
	./$(DECK_BUILDER)


.PHONY: all clean run run_deck_builder
