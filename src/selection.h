#ifndef SELECTION_H
#define SELECTION_H

#include <unistd.h>
#include "access.h"
#include "board_art.h"

/*=========================================================
    Selection Colors
=========================================================*/

#define HOVER_COLOR      LIGHT_GREEN
#define SELECTION_COLOR  RED

#define SELECT_CANCEL  (-1)
#define SELECT_PASS    (-2)

/*=========================================================
    Selection Option
=========================================================*/

typedef struct
{
    BoardPosition position;

    /*
        Normal outline color of the object at this position.
        Used to restore the outline when the cursor moves.
    */
    Color normal_color;

    // Actual slot represent by this option
    int index;

} SelectionOption;


/*=========================================================
    Selection
=========================================================*/

/*
    Displays a virtual cursor over a collection of positions.

    Controls:
        W / A / S / D
        Arrow keys

        Enter:
            Select current option.

        Escape:
            Cancel.

    Returns:
        >= 0 : index of selected option
        -1   : cancelled
*/
int select_target(
    const SelectionOption *options,
    int option_count);

#endif