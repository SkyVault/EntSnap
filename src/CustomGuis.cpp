#include "CustomGuis.hpp"

constexpr float MARGIN {4.0};

constexpr char delim = ';';

void GuiTabPanels(Rectangle reg, Vector2 tabSize, const char* tabNames, int* currentTab) {
    // Draw the tab bar
    const auto borderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));

    const auto backgroundColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    const auto backgroundColor2 = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));

    const auto [tabWidth, tabHeight] = tabSize;

    char* it = (char*)tabNames;
    char* last = it + strlen(tabNames);
    char* start = it;

    const auto [x, y, w, h] = reg;
    DrawRectangleLinesEx({x, y + tabHeight - 2, w, h - tabHeight}, 2, borderColor);

    int index = 0;
    while (it != last) {
        if ((*it) == delim) {
            char str[512];
            sprintf(str, "%.*s", (int)(it - start), start);

            const auto xx = x + (index * tabWidth) + (index * MARGIN);

            if (index != *currentTab) {
                DrawRectangle(xx, y, tabWidth, tabHeight, backgroundColor2);
            }

            DrawRectangleLinesEx({xx, y, tabWidth, tabHeight}, 2, borderColor);

            const auto region = Rectangle{xx + MARGIN*2, y, tabWidth, tabHeight};

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                if (CheckCollisionPointRec(GetMousePosition(), region))
                    *currentTab = index;

            GuiLabel(region, str);

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
}
