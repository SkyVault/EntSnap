#include <iostream>
#include <tuple>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS

#include <raygui.h>
#include <memory>
#include <optional>

#include "Project.hpp"

using namespace EntSnap;

constexpr std::tuple<int, int> WIN_SIZE {1280, 720};
constexpr float MARGIN {4.0};
constexpr float DMARGIN {MARGIN * 2};
constexpr float TOOL_BAR_H = 64.0;
constexpr float COMP_LIST_W = 300.0;

enum class ModalTypes {
    NEW_COMPONENT_MODAL,
    EXPORT_PROJECT_MODAL,
    IMPORT_PROJECT_MODAL,
};

class App {
public:
    Proj currentProj;

    std::optional<ModalTypes> currentModal{std::nullopt};
};

void DoToolbar(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    const float w = GetScreenWidth();
    const auto height = TOOL_BAR_H;
    auto xcr = xoff;
    auto ycr = yoff;
    const auto btnw =  height - DMARGIN * 2;

    GuiGroupBox({xcr + MARGIN, ycr + MARGIN, w - DMARGIN, height}, "Tools");

    GuiButton({xcr + MARGIN + DMARGIN, ycr + MARGIN + DMARGIN, btnw, btnw}, "New");
    xcr += btnw + MARGIN;
    GuiButton({xcr + MARGIN + DMARGIN, ycr + MARGIN + DMARGIN, btnw, btnw}, "Export");
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
        GuiPanel({xcr, ycr, w - DMARGIN, 64});
        GuiLabel({xcr + MARGIN, ycr, w - DMARGIN - MARGIN, 24}, comp.name.c_str());
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

    const auto width = (GetScreenWidth() - DMARGIN) - COMP_LIST_W;
    GuiGroupBox({xcr, ycr, width, (GetScreenHeight() - yoff) - MARGIN}, "Entity");
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

    static Component NewComponent = {.name=""};

    float xcr = x + MARGIN;
    float ycr = y + MARGIN;

    GuiDrawRectangle(bounds, 1, BLACK, WHITE);

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
    GuiTextBox(inputBounds, nameBuff, 32, editing);
    ycr += 24 + MARGIN;

    const float ww = (w - (DMARGIN + MARGIN * 3)) / 3.0f;
    const float preXcr = xcr;

    if (NewComponent.props.size() > 0) {
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

    for (auto& [name, obj]: NewComponent.props) {

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
        NewComponent.props.push_back({"", Object{static_cast<int>(Types::STRING), ""}});
    }
}

int main() {
    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    const auto [wx, wy] = WIN_SIZE;
    InitWindow(wx, wy, "EntSnap");
    SetExitKey(-1);

    auto app = std::make_unique<App>();

    auto body = Component{
        "Body",
        {  }
    };

    auto phy = Component{
            "Physics",
            {  }
    };

    auto longN = Component{
"ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ",
    {  }
    };

    app->currentProj.components.push_back(body);
    app->currentProj.components.push_back(phy);
    app->currentProj.components.push_back(longN);

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(WHITE);

        if (app->currentModal != std::nullopt) GuiLock();
        const auto tabIndex = DoTabBar(app, 0, 0);

        if (tabIndex == 0) {
            DoToolbar(app, 0, 32 + MARGIN);
            const auto yframe = 32 + DMARGIN + MARGIN + TOOL_BAR_H;
            DoComponentList(app, GetScreenWidth() - COMP_LIST_W, yframe);
            DoEntityView(app, MARGIN, yframe);
        }

        if (tabIndex == 1) {
            DoComponentCreator(app, 0, 32 + MARGIN);
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
