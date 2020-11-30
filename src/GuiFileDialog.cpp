#include "GuiFileDialog.hpp"

std::size_t number_of_files_in_directory(std::filesystem::path path) {
    int result = 0;
    for (auto &p : fs::directory_iterator(path)) 
        result += 1;
    return result;
}

FileDialogState GuiInitFileDialog(const std::string& initPath, bool active, const int width, const int height){ 
    FileDialogState state{};

    state.active = active;

    state.size = {
        width < 0 ? state.size.x : width,
        height < 0 ? state.size.y : height,
    };

    state.position = {
        GetScreenWidth() / 2 - state.size.x / 2, 
        GetScreenHeight() / 2 - state.size.y / 2,
    };

    state.path = initPath;
    sprintf(state.editPath, "%s", state.path.string().c_str());

    return state;
}

void GuiFileDialog(FileDialogState& state) {
    if (!state.active) return;

    const auto [winWidth, winHeight] = state.size;
    const auto [x, y] = state.position;

    state.active = !GuiWindowBox({x + 0, y + 0, winWidth, winHeight}, "#198#LuaJIT | Select File Dialog");

    auto setEditPath = [&state]() -> void { 
        sprintf(state.editPath, "%s", state.path.string().c_str());
    };

    if (GuiButton({x + winWidth - 50, y + 35, 40, 25}, "< ..")) {
        state.path = state.path.parent_path(); 
        setEditPath();
        state.filesListActive = -1;
    }

    if (GuiTextBox({x + 10, y + 35, winWidth - 65, 25}, state.editPath, 256, state.dirPathEditMode)) {
        if (state.dirPathEditMode) { 
            try {
                const auto path = fs::path(std::string{state.editPath});
                if (fs::exists(path)) state.path = path; 
                setEditPath();
            } catch(fs::filesystem_error& e) {
                std::cout << e.what() << std::endl;
                setEditPath();
            }
        }

        state.dirPathEditMode = !state.dirPathEditMode;
    }

    int prevTextAlignment = GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT);
    int prevElementsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);

    // Files list
    const auto bounds = Rectangle{x + 10, y + 70, winWidth - 20, winHeight - 135};

    int count = number_of_files_in_directory(state.path);
    int itemSelected = state.filesListActive;
    bool useScrollBar = false; 

    if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING))*count > bounds.height) 
        useScrollBar = true; 

    Rectangle itemBounds = {0};
    itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING);
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT); 
    if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

    // Get items on the list
    int visibleItems = bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
    if (visibleItems > count) visibleItems = count;

    int startIndex = (state.scrollIndex < 0) ? 0 : state.scrollIndex;
    if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
    int endIndex = startIndex + visibleItems;

    if ((GuiGetState() != GUI_STATE_DISABLED) && !GuiGetLocked()) { 
        const auto mousePoint = GetMousePosition(); 
        if (CheckCollisionPointRec(mousePoint, bounds)) {
            GuiSetState(GUI_STATE_FOCUSED);
            for (int i = 0; i < visibleItems; i++) {
                if (CheckCollisionPointRec(mousePoint, itemBounds)) {
                    state.itemFocused = startIndex + i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) itemSelected = startIndex + i;
                    break;
                } 
                itemBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING); 
            }
            if (useScrollBar) {
                int wheelMove = GetMouseWheelMove();
                startIndex -= wheelMove;

                if (startIndex < 0) startIndex = 0;
                else if (startIndex > (count - visibleItems)) startIndex = count - visibleItems;

                endIndex = startIndex + visibleItems;
                if (endIndex > count) endIndex = count;
            } 
        } else state.itemFocused = -1;
        itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH); 
    } 

    DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));     // Draw background
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, GuiGetState()*3)), 1.0f)); 

    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

    int index = 0;
    for (auto& dirEntry: fs::directory_iterator(state.path)) {
        if (index < startIndex) {
            index++;
            continue;
        }
        fs::path path = dirEntry.path();

        int icon = RICON_FILETYPE_TEXT; 
        if (dirEntry.is_directory())
            icon = RICON_FOLDER_FILE_OPEN;

        if (GuiGetState() == GUI_STATE_DISABLED) {
            if ((startIndex + index) == itemSelected) {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), 1.0f));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_DISABLED)), 1.0f)); 
            } 
            GuiLabel(itemBounds, path.filename().string().c_str());
        }
        else {
            if ((startIndex + index) == itemSelected) {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), 1.0f));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), 1.0f)); 
            } else if ((startIndex + index) == state.itemFocused) {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), 1.0f));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), 1.0f));
            }
            GuiLabel(itemBounds, GuiIconText(icon, path.filename().string().c_str())); 
        }

        itemBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING); 
        index += 1;
    }

    if (useScrollBar) {
        Rectangle scrollBarBounds = {
            bounds.x + bounds.width - GuiGetStyle(LISTVIEW, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.y + GuiGetStyle(LISTVIEW, BORDER_WIDTH), (float)GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH)
        };

        // Calculate percentage of visible items and apply same percentage to scrollbar
        float percentVisible = (float)(endIndex - startIndex)/count;
        float sliderSize = bounds.height*percentVisible;

        int prevSliderSize = GuiGetStyle(SCROLLBAR, SLIDER_WIDTH);  // Save default slider size
        int prevScrollSpeed = GuiGetStyle(SCROLLBAR, SCROLL_SPEED); // Save default scroll speed
        GuiSetStyle(SCROLLBAR, SLIDER_WIDTH, sliderSize);           // Change slider size
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, count - visibleItems); // Change scroll speed

        startIndex = GuiScrollBar(scrollBarBounds, startIndex, 0, count - visibleItems);
        std::cout << startIndex << std::endl;

        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, prevScrollSpeed); // Reset scroll speed to default
        GuiSetStyle(SCROLLBAR, SLIDER_WIDTH, prevSliderSize);  // Reset slider size to default 
    }

    EndScissorMode();

    state.scrollIndex = startIndex;
}