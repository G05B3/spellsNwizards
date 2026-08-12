#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdarg.h>

/*=========================================================
    ANSI Colors
=========================================================*/

typedef enum
{
    BLACK      = 16,
    WHITE      = 15,

    RED        = 196,
    GREEN      = 46,
    BLUE       = 21,

    YELLOW     = 226,
    ORANGE     = 208,
    PINK       = 205,
    PURPLE     = 135,
    CYAN       = 51,

    GRAY       = 245,
    DARK_GRAY  = 240,

    LIME       = 118,
    BROWN      = 94,

    MAGENTA     = 201,
    OLIVE = 154,

    /* Light / pastel colors */
    LIGHT_YELLOW = 230,
    LIGHT_CYAN   = 159,
    LIGHT_GREEN  = 157,
    LIGHT_BLUE   = 117,

} Color;


/*=========================================================
    Card dimensions
=========================================================*/

#define CLENGTH     20
#define CHEIGHT    12
#define CARD_SPACING    6

#define PALETTE_SIZE   16


/*=========================================================
    UTF-8 Box Drawing
=========================================================*/

/* Heavy */

#define TL  "╔"
#define TR  "╗"
#define BL  "╚"
#define BR  "╝"

#define HZ  "═"
#define VL  "║"

#define D1  "╠"
#define D2  "╣"

#define E1  "█"
#define E2  "▄"
#define E3  "▀"


/* Light */

#define LTL "┌"
#define LTR "┐"
#define LBL "└"
#define LBR "┘"

#define LHZ "─"
#define LVL "│"

#define LD1 "├"
#define LD2 "┤"

#define LC1 "┬"
#define LC2 "┴"
#define LCR "┼"


/*=========================================================
    Globals
=========================================================*/

extern int terminal_width;
extern int terminal_height;


/*=========================================================
    Cursor / Screen
=========================================================*/

void gotoxy(int x, int y);

void clear_screen(void);

void hide_cursor(void);

void show_cursor(void);


/*=========================================================
    Colors
=========================================================*/

void set_color(Color c);

void reset_color(void);

/*
    Converts

        "red"
        "bright_blue"

    into the corresponding enum.

    Returns -1 if invalid.
*/

int color_from_name(const char *name);


/*=========================================================
    Drawing Helpers
=========================================================*/

/*
    Draw a single colored string.

    Automatically restores the previous color.
*/

void draw_line(
    int x,
    int y,
    Color color,
    const char *text);


/*
    printf()-style version.
*/

void draw_text(
    int x,
    int y,
    Color color,
    const char *fmt,
    ...);


/*=========================================================
    Terminal Information
=========================================================*/

/*
    Updates

        terminal_width
        terminal_height
*/

void get_terminal_size(void);


/*
    Convenience helper.

    Returns how many cards fit horizontally.
*/

int cards_per_row(void);

void draw(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2);

void draw3(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3);

void draw4(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3,
    Color c4,
    const char *s4);

void draw5(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3,
    Color c4,
    const char *s4,
    Color c5,
    const char *s5);

void draw6(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3,
    Color c4,
    const char *s4,
    Color c5,
    const char *s5,
    Color c6,
    const char *s6);

void draw7(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3,
    Color c4,
    const char *s4,
    Color c5,
    const char *s5,
    Color c6,
    const char *s6,
    Color c7,
    const char *s7);

#endif