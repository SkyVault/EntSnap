#include "SidePanel.hpp"

bool DoComponentCard(
    std::unique_ptr<App>& app,
    const Component& comp,
    float xcr,
    float ycr,
    CmdState state,
    bool canEdit,
    bool canAttach) {
    const auto w = COMP_LIST_W - MARGIN;
    const Rectangle region = {xcr, ycr, w - DMARGIN, COMP_CARD_H};

    bool hot = !CheckCollisionPointRec(GetMousePosition(), region);

    DrawRectangleRec(region, canAttach
                         ? GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))
                         : Color{200, 200, 200, 255});
    DrawRectangleLinesEx(region,
                         1.0f,
                         GetColor(GuiGetStyle(DEFAULT, hot
                                              ? BORDER_COLOR_NORMAL
                                              : BORDER_COLOR_FOCUSED)));

    // Move triangle
    if (canAttach && state == CmdState::UNATTACHED)
        DrawLeftArrow({xcr + MARGIN, ycr + MARGIN, 24, 24},
            Color{200, 200, 200, 255});

    if (state == CmdState::ATTACHED) {
        DrawRightArrow({
            xcr + w - (DMARGIN + 24 + MARGIN),
            ycr + MARGIN, 24, COMP_CARD_H - DMARGIN},
            Color{200, 200, 200, 255});
    }

    GuiLabel({xcr + MARGIN, ycr, w - DMARGIN - MARGIN, 32}, comp.name.c_str());

    // Edit button
    if (canEdit && GuiButton({xcr + w - (96 + MARGIN*2), ycr + MARGIN, 94, 24}, "Edit")) {
        app->guiComponent = ToGuiComponent(comp);
        app->currentModal = ModalTypes::EDIT_COMPONENT_MODAL;
    }

    return
        canAttach &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        app->currentModal == std::nullopt &&
        CheckCollisionPointRec(GetMousePosition(), region);
}


void DoComponentTab(std::unique_ptr<App>& app, float xcr, float ycr) {
    const auto w = COMP_LIST_W - MARGIN;

    if (GuiButton({xcr, ycr, 100, 24}, "+ New Component")) {
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
        bool attached = false;
        if (app->isEditingEntity()) {
            attached = app->getCurrentEntity().has(comp.name);
        }
        if (DoComponentCard(app,
                            comp,
                            xcr,
                            ycr,
                            attached ? CmdState::ATTACHED : CmdState::UNATTACHED,
                            true,
                            !attached)) {
            if (app->isEditingEntity()) {
                app->getCurrentEntity().components.emplace_back(comp);
            }
        }
        ycr += 64 + MARGIN;
    } 
}

void DoEntitiesTab(std::unique_ptr<App>& app, float xcr, float ycr) { 
}

void DoProjectTab(std::unique_ptr<App>& app, float xcr, float ycr) { 
    const auto w = COMP_LIST_W - MARGIN;

    GuiLabel({xcr, ycr, 100, 24}, "Project Name"); 
    ycr += 24 + MARGIN;

    static InputState projNameState{};
    GuiStringInput(projNameState, {xcr, ycr, w - DMARGIN, 24}, app->currentProj.name, 32);
    ycr += 24 + MARGIN;


} 

void DoSidePanel(std::unique_ptr<App>& app, const float xoff, const float yoff) {
    const auto w = COMP_LIST_W - MARGIN;

    static int tab = 0;
    GuiTabPanels(
        {xoff, yoff, w, (GetScreenHeight() - yoff) - MARGIN + 2},
        {96.0f, 24.0f}, "Components;Entities;Project;", &tab, false);

    auto xcr = xoff + MARGIN;
    auto ycr = yoff + MARGIN + 24.0f + MARGIN;

    switch (tab) {
        case SIDE_PANEL_COMPONENTS: DoComponentTab(app, xcr, ycr); return;
        case SIDE_PANEL_ENTITIES: DoEntitiesTab(app, xcr, ycr); return;
        case SIDE_PANEL_PROJECT: DoProjectTab(app, xcr, ycr); return;
        default: return;
    } 
}

