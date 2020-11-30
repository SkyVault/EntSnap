#ifndef ENTSNAP_SIDE_PANEL_HPP
#define ENTSNAP_SIDE_PANEL_HPP

#include <memory>
#include <raylib.h>

#include "CustomGuis.hpp"
#include "Consts.hpp"
#include "App.hpp"

enum class CmdState {
    UNATTACHED,
    ATTACHED,
};

enum SidePanelTabs {
    SIDE_PANEL_COMPONENTS,
    SIDE_PANEL_ENTITIES,
    SIDE_PANEL_PROJECT
};
bool DoComponentCard(
    std::unique_ptr<App>& app,
    const Component& comp,
    float xcr,
    float ycr,
    CmdState state=CmdState::UNATTACHED,
    bool canEdit=false,
    bool canAttach=true);
void DoComponentTab(std::unique_ptr<App>& app, float xcr, float ycr);
void DoEntitiesTab(std::unique_ptr<App>& app, float xcr, float ycr);
void DoProjectTab(std::unique_ptr<App>& app, float xcr, float ycr);
void DoSidePanel(std::unique_ptr<App>& app, const float xoff, const float yoff);

#endif //ENTSNAP_SIDE_PANEL_HPP
