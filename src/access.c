#include "access.h"

#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>


/*=========================================================
    Globals
=========================================================*/

int terminal_width = 80;
int terminal_height = 24;

/*=========================================================
    Color Name Lookup
=========================================================*/

typedef struct
{
    const char *name;
    Color color;
    int ansi256;

} ColorEntry;

static const ColorEntry color_table[] =
{
    {"black",     BLACK,      16},
    {"white",     WHITE,      15},
    {"red",       RED,       196},
    {"green",     GREEN,      46},
    {"blue",      BLUE,       21},
    {"yellow",    YELLOW,    226},
    {"orange",    ORANGE,    208},
    {"cyan",      CYAN,       51},
    {"purple",    PURPLE,    135},
    {"pink",      PINK,      205},
    {"gray",      GRAY,      245},
    {"dark_gray", DARK_GRAY, 240},
    {"lime",      LIME,      118},
    {"brown",     BROWN,      94},
    {"magenta",    MAGENTA,   201},
    {"olive",     OLIVE,     154},
    {"light_yellow", LIGHT_YELLOW, 230},
    {"light_cyan",   LIGHT_CYAN,   159},
    {"light_green",  LIGHT_GREEN,  157},
    {"light_blue",   LIGHT_BLUE,   117}
};


/*=========================================================
    Cursor
=========================================================*/

void gotoxy(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}


void clear_screen(void)
{
    printf("\033[2J");
    printf("\033[H");
}


void hide_cursor(void)
{
    printf("\033[?25l");
}


void show_cursor(void)
{
    printf("\033[?25h");
}


/*=========================================================
    Colors
=========================================================*/

void set_color(Color c)
{
    printf("\033[38;5;%dm", c);
}


void reset_color(void)
{
    printf("\033[0m");
}


int color_from_name(const char *name)
{
    int i;

    for (i = 0;
         i < (int)(sizeof(color_table) / sizeof(color_table[0]));
         i++)
    {
        if (!strcmp(name, color_table[i].name))
            return color_table[i].color;
    }

    return -1;
}


/*=========================================================
    Drawing Helpers
=========================================================*/

void draw_line(
    int x,
    int y,
    Color color,
    const char *text)
{
    gotoxy(x, y);

    set_color(color);

    printf("%s", text);

    reset_color();
}


void draw_text(
    int x,
    int y,
    Color color,
    const char *fmt,
    ...)
{
    va_list args;

    gotoxy(x, y);

    set_color(color);

    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);

    reset_color();
}


/*=========================================================
    Terminal Size
=========================================================*/

void get_terminal_size(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO,
              TIOCGWINSZ,
              &ws) == 0)
    {
        terminal_width = ws.ws_col;
        terminal_height = ws.ws_row;
    }
}


int cards_per_row(void)
{
    int n;

    get_terminal_size();

    n = terminal_width /
        (CLENGTH + CARD_SPACING);

    if (n < 1)
        n = 1;

    return n;
}

void draw(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    reset_color();
}

void draw3(
    int x,
    int y,
    Color c1,
    const char *s1,
    Color c2,
    const char *s2,
    Color c3,
    const char *s3)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    set_color(c3);
    printf("%s", s3);

    reset_color();
}

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
    const char *s4)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    set_color(c3);
    printf("%s", s3);

    set_color(c4);
    printf("%s", s4);

    reset_color();
}

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
    const char *s5)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    set_color(c3);
    printf("%s", s3);

    set_color(c4);
    printf("%s", s4);

    set_color(c5);
    printf("%s", s5);

    reset_color();
}

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
    const char *s6)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    set_color(c3);
    printf("%s", s3);

    set_color(c4);
    printf("%s", s4);

    set_color(c5);
    printf("%s", s5);

    set_color(c6);
    printf("%s", s6);

    reset_color();
}

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
    const char *s7)
{
    gotoxy(x, y);

    set_color(c1);
    printf("%s", s1);

    set_color(c2);
    printf("%s", s2);

    set_color(c3);
    printf("%s", s3);

    set_color(c4);
    printf("%s", s4);

    set_color(c5);
    printf("%s", s5);

    set_color(c6);
    printf("%s", s6);

    set_color(c7);
    printf("%s", s7);

    reset_color();
}