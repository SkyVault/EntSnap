#ifndef ENTSNAP_CUSTOMGUIS_H
#define ENTSNAP_CUSTOMGUIS_H

#include <iostream>
#include <raylib.h>
#include <ricons.h>
#include <raygui.h>
#include <string>
#include <cstring>
#include <functional>

#include "Consts.hpp"

int GuiTabPanels(Rectangle reg, Vector2 tabSize, const char* tabNames, int* currentTab, bool closable=true);
void DrawX(Rectangle reg, float thick, Color color);
void DrawLeftArrow(Rectangle reg, Color color);
void DrawRightArrow(Rectangle reg, Color color);

struct InputState {
    bool editing {false};
    int framesCounter = 0;
    int cursor = 0;
    char buf[512];
};

void GuiInputBase(InputState& state, Rectangle bounds, std::string& val, const int textSize);
void GuiStringInput(InputState& state, Rectangle reg, std::string& val, const int fontSize);
void GuiStringInputValidate(
    InputState& state, 
    Rectangle reg,
    std::string& val,
    const int fontSize, 
    std::function<bool(const std::string&)>& validator);

#endif // ENTSNAP_CUSTOMGUIS_H_
