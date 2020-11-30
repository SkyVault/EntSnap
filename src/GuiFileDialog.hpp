#ifndef ENTSNAP_GUI_FILE_DIALOG_H
#define ENTSNAP_GUI_FILE_DIALOG_H

#include <iostream>
#include <raylib.h> 
#include <raygui.h>
#include <ricons.h>
#include <tuple>
#include <string>
#include <filesystem>

#include "CustomGuis.hpp"

namespace fs = std::filesystem;

struct FileDialogState {
    Vector2 position;
    Vector2 size{440, 310};

    fs::path filePath{""};
    fs::path path{""};
    char editPath[512] {'\0'};

    bool active{false};
    bool dirPathEditMode{false};

    bool filesListActive{false};
    int prevFilesListActive{-1};

    int scrollIndex{-1};
    int itemFocused{-1};
};

FileDialogState GuiInitFileDialog(const std::string& initPath, bool active=false, const int width=-1, const int height=-1);
void GuiFileDialog(FileDialogState& state);

#endif // GUI_FILE_DIALOG_IMPLEMENTATION
