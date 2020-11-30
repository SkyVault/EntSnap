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
#include "GuiFileDialog.hpp"

using namespace EntSnap;

namespace fs = std::filesystem;

enum class ModalTypes {
    NEW_PROJECT_MODAL,
    OPEN_PROJECT_MODAL,
    SAVE_AS_PROJECT_MODAL,
    NEW_COMPONENT_MODAL,
    EDIT_COMPONENT_MODAL,
    EXPORT_PROJECT_MODAL,
    IMPORT_PROJECT_MODAL,
};

class App {
public:
    App();

    Proj currentProj;
    GuiComponent guiComponent;

    FileDialogState fileDialogState;

    int editingEntityIndex{0};

    std::optional<ModalTypes> currentModal{std::nullopt};

    inline Ent& getCurrentEntity() {
        return currentProj.entities[editingEntityIndex];
    }

    inline bool isEditingEntity() {
        return currentProj.entities.size() > 0;
    }

    inline std::vector<Ent>& getEntities() {
        return currentProj.entities; 
    }

    void nextEntity();
    void prevEntity();

    void serializeProject(const std::filesystem::path& path);
    void loadProject(const std::filesystem::path& path);
};


#endif // ENTSNAP_APP_H
