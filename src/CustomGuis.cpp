#include "CustomGuis.hpp"

constexpr float MARGIN {4.0};
constexpr float DMARGIN {MARGIN*2};

constexpr char delim = ';';

int GuiTabPanels(Rectangle reg, Vector2 tabSize, const char* tabNames, int* currentTab) {
    // Draw the tab bar
    const auto borderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));

    const auto backgroundColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    const auto backgroundColor2 = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));

    const auto [tabWidth, tabHeight] = tabSize;

    char* it = (char*)tabNames;
    char* last = it + strlen(tabNames);
    char* start = it;

    const auto [x, y, w, h] = reg;

    DrawRectangleRec({x, y + tabHeight - 2, w, h - tabHeight}, backgroundColor);
    DrawRectangleLinesEx({x, y + tabHeight - 2, w, h - tabHeight}, 2, borderColor);

    int closeIndex = -1;
    int index = 0;
    while (it != last) {
        if ((*it) == delim) {
            char str[512];
            sprintf(str, "%.*s", (int)(it - start), start);

            const auto xx = x + (index * tabWidth) + (index * MARGIN);

            if (index != *currentTab) {
                DrawRectangle(xx, y, tabWidth, tabHeight, backgroundColor2);
            } else {
                DrawRectangle(xx, y, tabWidth, tabHeight, backgroundColor);
            }

            DrawRectangleLinesEx({xx, y, tabWidth, tabHeight}, 2, borderColor);

            const Rectangle region = {xx + MARGIN*2, y, tabWidth, tabHeight};

            GuiLabel(region, str);

            const auto bw = (tabHeight - DMARGIN);
            const Rectangle closeRegion = {
                xx + tabWidth - (bw + MARGIN) + 4,
                y + MARGIN + 4,
                bw - 8,
                bw - 8};

            DrawX(closeRegion, 2.0, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), closeRegion)){
                closeIndex = index;
            }

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), region))
                closeIndex = index;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                if (CheckCollisionPointRec(GetMousePosition(), region) && closeIndex != index)
                    *currentTab = index;

            if (index == *currentTab) {
                DrawRectangle(xx + 2, y + tabHeight - 2.0f,
                              tabWidth - 4.0f, 4.0f, backgroundColor);
            }

            it++;
            start = it;
            index += 1;
            continue;
        }
        it++;
    }

    return closeIndex;
}

void DrawX(Rectangle reg, float thick, Color color) {
    Vector2 startA{reg.x, reg.y};
    Vector2 endA{reg.x+reg.width, reg.y+reg.height};

    Vector2 startB{reg.x,reg.y+reg.height};
    Vector2 endB{reg.x+reg.width,reg.y};

    DrawLineV(startA, endA, color);
    DrawLineV(startB, endB, color);
}

void DrawLeftArrow(Rectangle reg, Color color) {
    const auto top = Vector2{reg.x + reg.width, reg.y};
    const auto center = Vector2{reg.x, reg.y + reg.height / 2.0f};
    const auto bottom = Vector2{reg.x + reg.width, reg.y + reg.height};
    DrawTriangleLines(top, center, bottom, color);
}

void DrawRightArrow(Rectangle reg, Color color) {
    const auto top = Vector2{reg.x, reg.y};
    const auto center = Vector2{reg.x + reg.width, reg.y + reg.height / 2.0f};
    const auto bottom = Vector2{reg.x, reg.y + reg.height};
    DrawTriangleLines(bottom, center, top, color);
}
