#ifndef ENTSNAP_APP_H
#define ENTSNAP_APP_H

#include <iostream>
#include <tuple>

#include <memory>
#include <optional>
#include <filesystem>

#include "Project.hpp"
#include "Entity.hpp"
#include "Component.hpp"

using namespace EntSnap;

enum class ModalTypes {
    NEW_COMPONENT_MODAL,
    EDIT_COMPONENT_MODAL,
    EXPORT_PROJECT_MODAL,
    IMPORT_PROJECT_MODAL,
};

class App {
public:
    Proj currentProj;
    GuiComponent guiComponent;

    std::vector<std::unique_ptr<Ent>> editingEntities{};
    int editingEntityIndex{0};

    std::optional<ModalTypes> currentModal{std::nullopt};

    inline std::unique_ptr<Ent>& getCurrentEntity() {
        return editingEntities[editingEntityIndex];
    }

    inline bool isEditingEntity() {
        return editingEntities.size() > 0;
    }

    void serializeProject(const std::filesystem::path& path);
    void loadProject(const std::filesystem::path& path);
};


#endif // ENTSNAP_APP_H
