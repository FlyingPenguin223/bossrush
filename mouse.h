#ifndef MOUSE_H
#define MOUSE_H

#include <raylib.h>

Vector2 get_mouse_position();
void draw_line(Vector2 start, Vector2 end, float thickness, Color color);

#endif
