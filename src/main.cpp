#include <iostream>
#include <filesystem>
#include <tuple>
#include <raylib.h>
#include <algorithm>
#include <sstream>

#include <memory>
#include <optional>

#include "Project.hpp"
#include "CustomGuis.hpp"
#include "Component.hpp"
#include "SidePanel.hpp"
#include "Consts.hpp"
#include "App.hpp"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

using namespace EntSnap;

void DoToolbar(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    const float w = GetScreenWidth();
    const auto height = TOOL_BAR_H;
    auto xcr = xoff;
    auto ycr = yoff;
    const auto btnw =  100;

    const auto region = Rectangle{xcr + MARGIN, ycr, w - DMARGIN, height};
    DrawRectangleRec(region, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    // GuiGroupBox(region, "Tools");

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "+ New Project")) {
    } 
    xcr += btnw + MARGIN;

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "Open Project")) {
    } 
    xcr += btnw + MARGIN;

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "Save Project")) {
    } 
    xcr += btnw + MARGIN; 

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "Save As Project")) {
        app->fileDialogState.active = true;
        app->currentModal = ModalTypes::SAVE_AS_PROJECT_MODAL;
    } 
    xcr += btnw + MARGIN; 

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "+ New Entity")) {
        app->editingEntityIndex = app->getEntities().size();
        app->getEntities().emplace_back(Ent{});
    } 
    xcr += btnw + MARGIN;

    if (GuiButton({xcr + DMARGIN, ycr + MARGIN, btnw, 24}, "Export Entities")) {
        app->serializeProject("testout");
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
    if (GuiButton({xcr, taby + MARGIN, tabw, tabh}, "Entity Designer")) { index = 0; }
    xcr += tabw + MARGIN;
    if (GuiButton({xcr, taby + MARGIN, tabw, tabh}, "Components")) { index = 1; }
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
    for (const auto& e : app->getEntities()) {
        if (e.name == "") {
           tabs  += "[ unnamed ];";
        } else {
            tabs += e.name + ";";
        }
    }

    ycr += MARGIN;
    int closeIndex =
        GuiTabPanels(
            Rectangle{xcr, ycr, width - MARGIN, height - MARGIN + 2},
            Vector2{96.0f, 24.0f},
            tabs.c_str(),
            &app->editingEntityIndex);

    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_TAB)) {
        app->nextEntity();
    }

    if (closeIndex >= 0) {
        app->getEntities().erase(app->getEntities().begin() + closeIndex);
        if (app->editingEntityIndex >= app->getEntities().size())
            app->editingEntityIndex = app->getEntities().size() - 1;
        return;
    }

    ycr += 24.0f;

    const auto xx = xcr + width - (COMP_LIST_W + DMARGIN + MARGIN);
    const auto yy = ycr + DMARGIN;
    GuiGroupBox({xx,
                yy, COMP_LIST_W, innerHeight - MARGIN},
                "Attached Components");
    // Draw attached components in a list
    int index = 0;
    {
        auto xcr = xx + MARGIN + MARGIN/2;
        auto ycr = yy + MARGIN + index * (MARGIN + COMP_CARD_H) + MARGIN/2;
        for (auto& comp : ent.components) { 
            const auto w = COMP_LIST_W - MARGIN;

            auto calcHeight = comp.props.size() * (24 + MARGIN); 

            const Rectangle region = {xcr, ycr, w - DMARGIN, COMP_CARD_H + calcHeight};

            bool hot = !CheckCollisionPointRec(GetMousePosition(), region);
            DrawRectangleRec(region, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            DrawRectangleLinesEx(
                region, 1.0f, GetColor(GuiGetStyle(DEFAULT, hot
                                              ? BORDER_COLOR_NORMAL
                                              : BORDER_COLOR_FOCUSED)));

            const auto arrowRegion = Rectangle{
                xcr + w - (DMARGIN + 24 + MARGIN),
                ycr + MARGIN, 24, 24};
            DrawRightArrow(arrowRegion, Color{200, 200, 200, 255});

            xcr += MARGIN;

            GuiLabel({xcr, ycr, w - DMARGIN - MARGIN, 32}, comp.name.c_str());

            ycr += 24 + DMARGIN;

            const auto nameW = (w - DMARGIN - MARGIN/2 - DMARGIN) * (1.0f/4.0f);
            const auto valueW = (w - DMARGIN - MARGIN/2 - DMARGIN) - nameW;

            auto xcrStart = xcr;
            for (auto& prop : comp.props) {
                switch(static_cast<Types>(prop.type)) { 
                    case Types::FLOAT: { 
                        GuiLabel({xcr, ycr, nameW, 24}, prop.name.c_str());
                        xcr += nameW + MARGIN; 
                        GuiInputBase(prop.inputState, {xcr, ycr, valueW, 24}, prop.value, 24); 
                        break;
                    }
                    case Types::STRING: {
                        break;
                    }
                    case Types::INTEGER: {
                        break;
                    }
                }
                xcr = xcrStart;
                ycr += 24 + MARGIN;
            }

            ycr += COMP_CARD_H - 24 - MARGIN;
            xcr = xcrStart - MARGIN;

            index++;
        } 
    }

    ycr += DMARGIN;
    xcr += DMARGIN;

    const auto ystart = ycr; 
    GuiLabel({xcr, ycr, 100, 24}, "Name"); 
    ycr += 24; 
    GuiStringInput(ent.nameInputState, {xcr, ycr, 200, 24}, ent.name, 32);

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

void DoNewComponentModal(std::unique_ptr<App>& app, bool edit=false) {
    const float w = 640;
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

    const float ww = ((w - (DMARGIN + MARGIN * 3)) / 3.0f) - (24 - (MARGIN * 3 + MARGIN));
    const float preXcr = xcr;

    const float factor = (1.0f / 1.5f);

    if (app->guiComponent.props.size() > 0) {
        GuiLabel({xcr, ycr, ww * factor, 24}, "Name");
        xcr += ww * factor + MARGIN;
        GuiLabel({xcr, ycr, ww * factor, 24}, "Type");
        xcr += ww * factor + MARGIN;
        GuiLabel({xcr, ycr, ww, 24}, "Value");
        xcr = preXcr;

        ycr += MARGIN;
        DrawLine(xcr, ycr + 14 + MARGIN, xcr + w - DMARGIN, ycr + 14 + MARGIN, BLACK);
        ycr += 24;
    }

    int remove = -1;

    int index = 0;
    for (auto& obj: app->guiComponent.props) {
        const auto nameRegion = Rectangle{xcr, ycr, ww * factor, 24};
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            obj.guiNameEdit = CheckCollisionPointRec(GetMousePosition(), nameRegion);
        GuiTextBox(nameRegion, obj.guiName, 32, obj.guiNameEdit);

        xcr += ww * factor + MARGIN;
        const auto dropDownRegion = Rectangle{xcr, ycr, ww * factor, 24};
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            if (CheckCollisionPointRec(GetMousePosition(), dropDownRegion))
                obj.guiTypeDropdown = true;

        std::stringstream ss("");
        int i = 0;
        for (const auto& t : TypesS) {
            ss << t;
            if (i < TypesS.size() - 1) ss << ";";
            i++;
        }

        if (GuiDropdownBox(dropDownRegion,
                           ss.str().c_str(), &obj.type, obj.guiTypeDropdown))
            obj.guiTypeDropdown = false;
        xcr += ww * factor + MARGIN;

        switch (static_cast<Types>(obj.type)) {
            case Types::STRING:
            case Types::FLOAT:
            case Types::INTEGER: {
                const auto valueRegion = Rectangle{xcr, ycr, ww * (1.0f / factor), 24}; 
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    obj.guiValueEdit = CheckCollisionPointRec(GetMousePosition(), valueRegion);
                GuiTextBox(valueRegion, obj.guiValue, 32, obj.guiValueEdit);
                break;
            }
            case Types::VECTOR_2: {

            }
        }

        xcr += ww * (1.0f / factor) + MARGIN;
        if (GuiButton({xcr, ycr, 24, 24}, "X")) {
            remove = index;
        }

        xcr = preXcr;
        ycr += 24 + MARGIN;
        index++;
    }

    if (remove >= 0) {
        app->guiComponent.props.erase(
            app->guiComponent.props.begin() + remove);
    }

    if (GuiButton({xcr, ycr, 100, 24}, "+ New Prop")) {
        app->guiComponent.props.push_back(GuiObject{});
    }

    if (edit && GuiButton({xcr, y + h - (24 + MARGIN), 96, 24}, "Delete")) {
        app->currentModal = std::nullopt;

        // TODO: Do are you sure modal
        app->currentProj.components.erase(
            std::remove_if(
                app->currentProj.components.begin(),
                app->currentProj.components.end(),
                [&app](const Component& t){
                    return t.name == std::string{ app->guiComponent.name };
                }
            )
        );
    }

    if (GuiButton({x + w - (120 + MARGIN)*2, y + h - (24 + MARGIN), 120, 24}, "Cancel")) {
        app->currentModal = std::nullopt;
    }

    if (GuiButton(
        {x + w - (120 + MARGIN), y + h - (24 + MARGIN), 120, 24},
        edit ? "Update" : "Create Component")) {
        const auto c = ToComponent(app->guiComponent);

        if (edit) {
            app->currentProj.components.erase(
                std::remove_if(
                    app->currentProj.components.begin(),
                    app->currentProj.components.end(),
                    [&c](const Component& t){ return t.name == c.name; }
                )
            );
            app->guiComponent.reset();
        }

        app->currentProj.components.emplace_back(c);
        app->guiComponent.reset();
        app->currentModal = std::nullopt;
    }
}

void DoSaveAsProjectModal(std::unique_ptr<App>& app) {
    GuiFileDialog(app->fileDialogState);

    if (!app->fileDialogState.active) {
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

    // app->loadProject("testout");

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        //GuiGrid((Rectangle){0,0,GetScreenWidth(),GetScreenHeight()},20.0f, 2); // draw a fancy grid

        if (app->currentModal != std::nullopt) GuiLock();

        const auto yframe = DMARGIN + MARGIN + TOOL_BAR_H;
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      Color{200, 200, 200, 255});
        DoToolbar(app, 0, MARGIN);
        DoSidePanel(app, GetScreenWidth() - COMP_LIST_W, yframe);

        if (app->getEntities().size() > 0) {
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
                case ModalTypes::EDIT_COMPONENT_MODAL: DoNewComponentModal(app, true); break;
                case ModalTypes::SAVE_AS_PROJECT_MODAL: DoSaveAsProjectModal(app); break;
                default: break;
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
