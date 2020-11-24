#include <iostream>
#include <tuple>
#include <raylib.h>

#include <memory>
#include <optional>

#include "Project.hpp"
#include "CustomGuis.hpp"
#include "App.hpp"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

using namespace EntSnap;

constexpr std::tuple<int, int> WIN_SIZE {1280, 720};
constexpr float MARGIN {4.0};
constexpr float DMARGIN {MARGIN * 2};
constexpr float TOOL_BAR_H = 64.0;
constexpr float COMP_LIST_W = 300.0;

void DoToolbar(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    const float w = GetScreenWidth();
    const auto height = TOOL_BAR_H;
    auto xcr = xoff;
    auto ycr = yoff;
    const auto btnw =  height - DMARGIN * 2;

    GuiGroupBox({xcr + MARGIN, ycr + MARGIN, w - DMARGIN, height}, "Tools");

    if (GuiButton({xcr + MARGIN + DMARGIN, ycr + MARGIN + DMARGIN, btnw, btnw}, "New")) {
        app->editingEntityIndex = app->editingEntities.size();
        app->editingEntities.emplace_back(std::make_unique<Ent>());
    }

    xcr += btnw + MARGIN;
    if (GuiButton({xcr + MARGIN + DMARGIN, ycr + MARGIN + DMARGIN, btnw, btnw}, "Export")) {
        app->serializeProject("testout");
    }
}

enum class CmdState {
    UNATTACHED,
    ATTACHED,
};

void DoComponentCard(std::unique_ptr<App>& app, const Component& comp, float xcr, float ycr, CmdState state=CmdState::UNATTACHED) {
    const auto w = COMP_LIST_W - MARGIN;
    GuiPanel({xcr, ycr, w - DMARGIN, 64});
    GuiLabel({xcr + MARGIN, ycr, w - DMARGIN - MARGIN, 24}, comp.name.c_str());
}

void DoComponentList(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    const auto w = COMP_LIST_W - MARGIN;
    GuiGroupBox({xoff, yoff, w, (GetScreenHeight() - yoff) - MARGIN}, "Components");

    auto xcr = xoff + MARGIN;
    auto ycr = yoff + MARGIN;

    if (GuiButton({xcr, ycr, 100, 24}, "New Component")) {
        app->currentModal = ModalTypes::NEW_COMPONENT_MODAL;
    }

    ycr += 24;

    static char search[512] = {'\0'};
    static int editing = false;
    GuiLabel({xcr, ycr, w - DMARGIN, 24}, "Search");
    ycr += 14 + MARGIN;
    auto inputBounds = Rectangle{xcr, ycr, w - DMARGIN, 24};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        editing = (CheckCollisionPointRec(GetMousePosition(), inputBounds));
    GuiTextBox(inputBounds, search, 32, editing);
    ycr += 24 + MARGIN;

    for (const auto& comp : app->currentProj.components) {
        DoComponentCard(app, comp, xcr, ycr);
        ycr += 64 + MARGIN;
    }
}

int DoTabBar(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    static int index = 0;

    const auto tabw = 100;
    const auto height = 32;
    const auto tabh = height - MARGIN;

    auto xcr = xoff;
    auto ycr = yoff;

    const auto taby = xcr + (height-tabh);
    if (GuiButton({xcr + MARGIN, taby + MARGIN, tabw, tabh}, "Entity Designer")) { index = 0; }
    xcr += tabw + MARGIN;
    if (GuiButton({xcr + MARGIN, taby + MARGIN, tabw, tabh}, "Components")) { index = 1; }
    return index;
}

void DoEntityView(std::unique_ptr<App>& app, const int xoff, const int yoff) {
    float xcr = xoff;
    float ycr = yoff;

    auto& ent = app->getCurrentEntity();

    const auto width = (GetScreenWidth() - MARGIN) - COMP_LIST_W;
    const auto height = (GetScreenHeight() - yoff) - MARGIN;
    const auto innerHeight = (GetScreenHeight() - yoff) - (DMARGIN * 2 + MARGIN) - (24 + MARGIN);

    std::string tabs{""};
    for (const auto& e : app->editingEntities) {
        if (e->name == "") {
           tabs  += "<unnamed>;";
        } else {
            tabs += e->name + ";";
        }
    }

    ycr += MARGIN;
    GuiTabPanels(Rectangle{xcr, ycr, width - MARGIN, height - MARGIN + 2},
                 Vector2{96.0f, 24.0f},
                 tabs.c_str(),
                 &app->editingEntityIndex);

    ycr += 24.0f;

    GuiGroupBox({xcr + width - (COMP_LIST_W + DMARGIN + MARGIN),
                 ycr + DMARGIN,
                 COMP_LIST_W,
                 innerHeight - MARGIN},
                 "Attached Components");

    ycr += DMARGIN;
    xcr += DMARGIN;

    const auto ystart = ycr;
    char nameBuf[512];
    { int i = 0; for (auto c : ent->name) nameBuf[i++] = c; }
    nameBuf[ent->name.size()] = '\0';

    GuiLabel({xcr, ycr, 100, 24}, "Name");

    ycr += 24;
    auto inputBounds = Rectangle{xcr, ycr, 200, 24};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        ent->editingName = (CheckCollisionPointRec(GetMousePosition(), inputBounds));
    GuiTextBox(inputBounds, nameBuf, 32, ent->editingName);

    ent->name = std::string{nameBuf};

    ycr += 24 + DMARGIN;
    GuiPanel({xcr, ycr, width - (COMP_LIST_W + DMARGIN * 3 + MARGIN), innerHeight - (ycr - ystart + MARGIN)});
}

void DoNoEntityView(std::unique_ptr<App>& app, const int xoff, const int yoff) {
    float xcr = xoff;
    float ycr = yoff;
    const auto width = (GetScreenWidth() - DMARGIN) - COMP_LIST_W;
    const auto height = 30;
    const auto height2 = 20;

    const auto y = GetScreenHeight() / 2 - height / 2;
    const auto text = "No entity being worked on";
    const auto text2 = "click new to create a new entity";

    const auto twidth = MeasureText(text, height);
    const auto twidth2 = MeasureText(text2, height2);

    DrawText(text, xcr + width / 2 - twidth / 2, y, height, GRAY);
    DrawText(text2, xcr + width / 2 - twidth2 / 2, y + height + 4, height2, LIGHTGRAY);
}

void DoComponentCreator(std::unique_ptr<App>& app, const int xoff, const int yoff) {
    DoComponentList(app, xoff + GetScreenWidth() - COMP_LIST_W, TOOL_BAR_H);
}

void DoNewComponentModal(std::unique_ptr<App>& app) {
    const float w = GetScreenWidth() * (1.0/3.0);
    const float h = GetScreenHeight() - 100;
    const float x = GetScreenWidth() / 2 - w / 2;
    const float y = GetScreenHeight() / 2 - h / 2;

    const Rectangle bounds = {x, y, w, h};

    float xcr = x + MARGIN;
    float ycr = y + MARGIN;

    GuiDrawRectangle(bounds, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    GuiLabel({xcr, ycr, 100, 24}, "Create a New Component");
    ycr += 24;
    DrawLine(xcr, ycr, xcr + w - DMARGIN, ycr, BLACK);
    ycr += MARGIN;

    static char nameBuff[512];
    static int editing = false;
    GuiLabel({xcr, ycr, w - DMARGIN, 24}, "Name");
    ycr += 14 + MARGIN;
    auto inputBounds = Rectangle{xcr, ycr, w - DMARGIN, 24};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        editing = (CheckCollisionPointRec(GetMousePosition(), inputBounds));
    GuiTextBox(inputBounds, app->guiComponent.name, 32, editing);
    ycr += 24 + MARGIN;

    const float ww = (w - (DMARGIN + MARGIN * 3)) / 3.0f;
    const float preXcr = xcr;

    if (app->guiComponent.props.size() > 0) {
        GuiLabel({xcr, ycr, ww, 24}, "Name");
        xcr += ww + MARGIN;
        GuiLabel({xcr, ycr, ww, 24}, "Type");
        xcr += ww + MARGIN;
        GuiLabel({xcr, ycr, ww, 24}, "Value");
        xcr = preXcr;

        ycr += MARGIN;
        DrawLine(xcr, ycr + 14 + MARGIN, xcr + w - DMARGIN, ycr + 14 + MARGIN, BLACK);
        ycr += 24;
    }

    for (auto& obj: app->guiComponent.props) {
        const auto nameRegion = Rectangle{xcr, ycr, ww, 24};
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            obj.guiNameEdit = CheckCollisionPointRec(GetMousePosition(), nameRegion);
        GuiTextBox({xcr, ycr, ww, 24}, obj.guiName, 32, obj.guiNameEdit);

        xcr += ww + MARGIN;
        const auto dropDownRegion = Rectangle{xcr, ycr, ww, 24};
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            if (CheckCollisionPointRec(GetMousePosition(), dropDownRegion))
                obj.guiTypeDropdown = true;

        if (GuiDropdownBox(dropDownRegion,
                           "STRING;FLOAT;INTEGER", &obj.type, obj.guiTypeDropdown))
            obj.guiTypeDropdown = false;
        xcr += ww + MARGIN;

        const auto valueRegion = Rectangle{xcr, ycr, ww, 24};
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            obj.guiValueEdit = CheckCollisionPointRec(GetMousePosition(), valueRegion);
        GuiTextBox(valueRegion, obj.guiValue, 32, obj.guiValueEdit);

        xcr = preXcr;
        ycr += 24 + MARGIN;
    }

    if (GuiButton({xcr, ycr, 100, 24}, "New Prop")) {
        app->guiComponent.props.push_back(GuiObject{});
    }

    if (GuiButton({x + w - (120 + MARGIN), y + h - (24 + MARGIN), 120, 24}, "Create Component")) {
        app->currentProj.components.emplace_back(app->guiComponent.toComponent());
        app->guiComponent.reset();
        app->currentModal = std::nullopt;
    }
}

int main() {
    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const auto [wx, wy] = WIN_SIZE;
    InitWindow(wx, wy, "EntSnap");
    SetExitKey(-1);

    //GuiLoadStyle("res/style");
    GuiLoadStyleDefault();
    GuiSetFont(GetFontDefault());

    auto app = std::make_unique<App>();
    app->loadProject("testout");

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        //GuiGrid((Rectangle){0,0,GetScreenWidth(),GetScreenHeight()},20.0f, 2); // draw a fancy grid

        if (app->currentModal != std::nullopt) GuiLock();

        DoToolbar(app, 0, MARGIN);
        const auto yframe = DMARGIN + MARGIN + TOOL_BAR_H;
        DoComponentList(app, GetScreenWidth() - COMP_LIST_W, yframe);

        if (app->editingEntities.size() > 0) {
            DoEntityView(app, MARGIN, yframe - MARGIN);
        } else {
            DoNoEntityView(app, MARGIN, yframe);
        }

        GuiUnlock();

        if (app->currentModal != std::nullopt) {
            DrawRectangle(
                0,
                0,
                GetScreenWidth(),
                GetScreenHeight(),
                Color{0, 0, 0, 135});

            switch (app->currentModal.value()) {
                case ModalTypes::NEW_COMPONENT_MODAL: DoNewComponentModal(app); break;
                default: break;
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
