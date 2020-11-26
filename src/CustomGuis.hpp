#ifndef ENTSNAP_CUSTOMGUIS_H
#define ENTSNAP_CUSTOMGUIS_H

#include <iostream>
#include <raylib.h>
#include <raygui.h>
#include <string>
#include <cstring>

int GuiTabPanels(Rectangle reg, Vector2 tabSize, const char* tabNames, int* currentTab);
void DrawX(Rectangle reg, float thick, Color color);
void DrawLeftArrow(Rectangle reg, Color color);
void DrawRightArrow(Rectangle reg, Color color);

#endif // ENTSNAP_CUSTOMGUIS_H_
