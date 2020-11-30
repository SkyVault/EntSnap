#include "CustomGuis.hpp"

int GuiTabPanels(Rectangle reg, Vector2 tabSize, const char* tabNames, int* currentTab, bool closable) {
    // Draw the tab bar
    const auto borderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));

    const auto backgroundColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    const auto backgroundColor2 = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));

    const auto [tabWidth, tabHeight] = tabSize;

    char* it = (char*)tabNames;
    char* last = it + strlen(tabNames);
    char* start = it;

    const auto [x, y, w, h] = reg;

    DrawRectangleRec({x, y + tabHeight - 2, w, h - tabHeight}, backgroundColor);
    DrawRectangleLinesEx({x, y + tabHeight - 2, w, h - tabHeight}, 2, borderColor);

    int closeIndex = -1;
    int index = 0;
    while (it != last) {
        if ((*it) == delim) {
            char str[512];
            sprintf(str, "%.*s", (int)(it - start), start);

            const auto xx = x + (index * tabWidth) + (index * MARGIN);

            if (index != *currentTab) {
                DrawRectangle(xx, y, tabWidth, tabHeight, backgroundColor2);
            } else {
                DrawRectangle(xx, y, tabWidth, tabHeight, backgroundColor);
            }

            DrawRectangleLinesEx({xx, y, tabWidth, tabHeight}, 2, borderColor);

            const Rectangle region = {xx + MARGIN*2, y, tabWidth, tabHeight};

            GuiLabel(region, str);

            const auto bw = (tabHeight - DMARGIN);
            const Rectangle closeRegion = {
                xx + tabWidth - (bw + MARGIN) + 4,
                y + MARGIN + 4,
                bw - 8,
                bw - 8};

            if (closable) {
                DrawX(closeRegion, 2.0, BLACK);
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), closeRegion)){
                closeIndex = index;
            }

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), region))
                closeIndex = index;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                if (CheckCollisionPointRec(GetMousePosition(), region) && closeIndex != index)
                    *currentTab = index;

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

    return closeIndex;
}

void DrawX(Rectangle reg, float thick, Color color) {
    Vector2 startA{reg.x, reg.y};
    Vector2 endA{reg.x+reg.width, reg.y+reg.height};

    Vector2 startB{reg.x,reg.y+reg.height};
    Vector2 endB{reg.x+reg.width,reg.y};

    DrawLineV(startA, endA, color);
    DrawLineV(startB, endB, color);
}

void DrawLeftArrow(Rectangle reg, Color color) {
    const auto top = Vector2{reg.x + reg.width, reg.y};
    const auto center = Vector2{reg.x, reg.y + reg.height / 2.0f};
    const auto bottom = Vector2{reg.x + reg.width, reg.y + reg.height};
    DrawTriangleLines(top, center, bottom, color);
}

void DrawRightArrow(Rectangle reg, Color color) {
    const auto top = Vector2{reg.x, reg.y};
    const auto center = Vector2{reg.x + reg.width, reg.y + reg.height / 2.0f};
    const auto bottom = Vector2{reg.x, reg.y + reg.height};
    DrawTriangleLines(bottom, center, top, color);
}

bool GuiTextEditor(Rectangle bounds, char *text, int textSize, bool editMode) {
    static Rectangle cursor = { 0 };    // Cursor position and size
    static int framesCounter = 0;       // Blinking cursor frames counter
    static int cursorCodepoint = -1;
    static int selectStartCp = -1;
    static int selectLengthCp = 0;

    bool guiLocked = GuiGetLocked();
    
    GuiControlState state = (GuiControlState)GuiGetState();
    bool pressed = false;
    
    bool textWrap = true;           // TODO: Word-Wrap vs Char-Wrap -> textWrapMode { NO_WRAP_LOCK, NO_WRAP_OVERFLOW, CHAR_WRAP, WORD_WRAP }

    // WARNING: First string full traversal
    int codepointCount = GetCodepointsCount(text);
    
    int textLen = strlen(text);     // Text length in bytes

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            framesCounter++;
            
            // Characters selection logic
            if (selectStartCp != -1)
            {
                if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_RIGHT))
                {
                    selectLengthCp++;
                    if (selectLengthCp >= (codepointCount - selectStartCp)) selectLengthCp = codepointCount - selectStartCp;
                }
                
                if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_LEFT))
                {
                    selectLengthCp--;
                    if (selectLengthCp < 0) selectLengthCp = 0;
                }
            }
            
            int key = GetKeyPressed();
            
            // TODO: On key pressed, place new character in cursor position

            // Exit edit mode logic
            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(0))) pressed = true;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(0)) pressed = true;
            }
        }

        if (pressed) 
        {
            // Exiting edit mode, reset temp variables
            framesCounter = 0;
            cursor = Rectangle{ 0 };
            
            cursorCodepoint = -1;
            selectStartCp = -1;
            selectLengthCp = 0;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, (state*3))), 1.0f));

    if (state == GUI_STATE_PRESSED) DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), 1.0f));
    else if (state == GUI_STATE_DISABLED) DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), 1.0f));

    Font font = GetFontDefault();
    
    int textOffsetY = 0;            // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw
    
    float scaleFactor = GuiGetStyle(DEFAULT, TEXT_SIZE)*2/font.baseSize;     // Character quad scaling factor
    
    for (int i = 0, cp = 0; i < textLen; i++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetNextCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);
        
        Rectangle rec = { bounds.x + textOffsetX + font.chars[index].offsetX*scaleFactor,
                          bounds.y + textOffsetY + font.chars[index].offsetY*scaleFactor, 
                          font.recs[index].width*scaleFactor, font.recs[index].height*scaleFactor };
                          
        // Automatic line break to wrap text inside box
        if (textWrap && ((rec.x + rec.width) >= (bounds.x + bounds.width)))
        {
            textOffsetY += (int)((font.baseSize + font.baseSize/2)*scaleFactor);
            textOffsetX = 0.0f;
            
            // Recalculate drawing rectangle position
            rec = Rectangle{ bounds.x + textOffsetX + font.chars[index].offsetX*scaleFactor,
                             bounds.y + textOffsetY + font.chars[index].offsetY*scaleFactor, 
                             font.recs[index].width*scaleFactor, font.recs[index].height*scaleFactor };
        }
        
        // Check selected codepoint
        if (editMode)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rec))
            {
                cursor = rec;
                cursorCodepoint = cp;
                selectStartCp = cursorCodepoint;
                selectLengthCp = 0;
                
                // TODO: Place cursor at the end if pressed out of text
            }
            
            // On mouse left button down allow text selection
            if ((selectStartCp != -1) && IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rec))
            {
                if (cp >= selectStartCp) selectLengthCp = cp - selectStartCp;
                else if (cp < selectStartCp) 
                { 
                    //int temp = selectStartCp;
                    //selectStartCp = cp;
                    //selectLengthCp = temp - selectStartCp;
                }
            }
        } 
        
        if (codepoint == '\n')    // Line break character
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += (int)((font.baseSize + font.baseSize/2)*scaleFactor);
            textOffsetX = 0.0f;
        }
        else
        {
            // Draw codepoint glyph
            if ((codepoint != ' ') && (codepoint != '\t') && ((rec.x + rec.width) < (bounds.x + bounds.width)))
            {
                DrawTexturePro(font.texture, font.recs[index], rec, Vector2{ 0, 0 }, 0.0f, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
            }
            
            // TODO: On text overflow do something... move text to the left?
        }
        
        // Draw codepoints selection from selectStartCp to selectLengthCp
        // TODO: Consider spacing when drawing selected characters background
        if (editMode && (selectStartCp != -1) && ((cp >= selectStartCp) && (cp <= (selectStartCp + selectLengthCp)))) DrawRectangleRec(rec, MAROON);
        
        if (font.chars[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width*scaleFactor + GuiGetStyle(DEFAULT, TEXT_SPACING));
        else textOffsetX += ((float)font.chars[index].advanceX*scaleFactor + GuiGetStyle(DEFAULT, TEXT_SPACING));
        
        i += (codepointByteCount - 1);   // Move text bytes counter to next codepoint
        cp++;
    }
    
    // Draw blinking cursor
    if (editMode && ((framesCounter/20)%2 == 0)) {
        DrawRectangleRec(cursor, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), 1.0f));
    }

    //GuiDrawText(text, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

int GetTextWidth(const char *text) {
    Vector2 size = { 0 };
    const auto& guiFont = GuiGetFont(); 
    if ((text != NULL) && (text[0] != '\0')) 
        size = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)); 
    return (int)size.x;
}

void GuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color) {
    if (color.a > 0) 
        DrawRectangle(rec.x, rec.y, rec.width, rec.height, color); 
    
    if (borderWidth > 0) {
        DrawRectangle(rec.x, rec.y, rec.width, borderWidth, borderColor);
        DrawRectangle(rec.x, rec.y + borderWidth, borderWidth, rec.height - 2*borderWidth, borderColor);
        DrawRectangle(rec.x + rec.width - borderWidth, rec.y + borderWidth, borderWidth, rec.height - 2*borderWidth, borderColor);
        DrawRectangle(rec.x, rec.y + rec.height - borderWidth, rec.width, borderWidth, borderColor);
    }
} 

const char *GetTextIcon(const char *text, int *iconId) {
    *iconId = -1;
    if (text[0] == '#') {
        char iconValue[4] = { 0 };

        int pos = 1;
        while ((pos < 4) && (text[pos] >= '0') && (text[pos] <= '9')) {
            iconValue[pos - 1] = text[pos];
            pos++;
        }

        if (text[pos] == '#') {
            *iconId = TextToInteger(iconValue); 
            if (*iconId >= 0) text += (pos + 1);
        }
    }
    return text;
}

void GuiDrawText(const char *text, Rectangle bounds, int alignment, Color tint) {
    #define TEXT_VALIGN_PIXEL_OFFSET(h)  ((int)h%2)     // Vertical alignment for pixel perfect

    if ((text != NULL) && (text[0] != '\0'))
    {
        int iconId = 0;
        text = GetTextIcon(text, &iconId);  // Check text for icon and move cursor

        // Get text position depending on alignment and iconId
        //---------------------------------------------------------------------------------
        #define ICON_TEXT_PADDING   4

        Vector2 position = { bounds.x, bounds.y };

        // NOTE: We get text size after icon been processed
        int textWidth = GetTextWidth(text);
        int textHeight = GuiGetStyle(DEFAULT, TEXT_SIZE);

        if (iconId >= 0) {
            textWidth += RICON_SIZE;
            if ((text != NULL) && (text[0] != '\0')) textWidth += ICON_TEXT_PADDING;
        }

        switch (alignment) {
            case GUI_TEXT_ALIGN_LEFT: {
                position.x = bounds.x;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_CENTER: {
                position.x = bounds.x + bounds.width/2 - textWidth/2;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_RIGHT: {
                position.x = bounds.x + bounds.width - textWidth;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            default: break;
        }

        position.x = (float)((int)position.x);
        position.y = (float)((int)position.y);

        if (iconId >= 0) {
            // GuiDrawIcon(iconId, RAYGUI_CLITERAL(Vector2){ position.x, bounds.y + bounds.height/2 - RICON_SIZE/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height) }, 1, tint);
            position.x += (RICON_SIZE + ICON_TEXT_PADDING);
        }

        DrawTextEx(
            GuiGetFont(), 
            text, position, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), tint);
        //---------------------------------------------------------------------------------
    }
}

Rectangle GetTextBounds(int control, Rectangle bounds) {
    Rectangle textBounds = bounds;

    textBounds.x = bounds.x + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.y = bounds.y + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.width = bounds.width - 2*GuiGetStyle(control, BORDER_WIDTH);
    textBounds.height = bounds.height - 2*GuiGetStyle(control, BORDER_WIDTH);

    switch (control) {
        case COMBOBOX: bounds.width -= (GuiGetStyle(control, COMBO_BUTTON_WIDTH) + GuiGetStyle(control, COMBO_BUTTON_PADDING)); break;
        case VALUEBOX: break;   // NOTE: ValueBox text value always centered, text padding applies to label
        default: {
            if (GuiGetStyle(control, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT) textBounds.x -= GuiGetStyle(control, TEXT_PADDING);
            else textBounds.x += GuiGetStyle(control, TEXT_PADDING);
        } break;
    }

    return textBounds;
} 

void GuiInputBase(InputState& istate, Rectangle bounds, std::string& val, const int textSize) {
    GuiControlState state = static_cast<GuiControlState>(GuiGetState());
    bool pressed = false;

    sprintf(istate.buf, "%s", val.c_str());

    Rectangle cursor = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(istate.buf) + 2,
        bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        1,
        GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !GuiGetLocked()) {
        Vector2 mousePoint = GetMousePosition();

        if (istate.editing) {
            state = GUI_STATE_PRESSED;
            istate.framesCounter++;

            int key = GetKeyPressed();      // Returns codepoint as Unicode
            int keyCount = strlen(istate.buf);

            // Only allow keys in range [32..125]
            if (keyCount < (textSize - 1)) {
                int maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)*2));

                if ((GetTextWidth(istate.buf) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE))) && (key >= 32)) {
                    int byteLength = 0;
                    const char *textUtf8 = CodepointToUtf8(key, &byteLength);
                    
                    for (int i = 0; i < byteLength; i++) {
                        istate.buf[keyCount] = textUtf8[i];
                        keyCount++;
                    }

                    istate.buf[keyCount] = '\0';
                }
            }

            // Delete text
            if (keyCount > 0) {
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    keyCount--;
                    istate.buf[keyCount] = '\0';
                    istate.framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                }
                else if (IsKeyDown(KEY_BACKSPACE)) {
                    if ((istate.framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (istate.framesCounter%2) == 0) keyCount--;
                    istate.buf[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                }
            }

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) pressed = true;

            // Check text alignment to position cursor properly
            int textAlignment = GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT);
            if (textAlignment == GUI_TEXT_ALIGN_CENTER) cursor.x = bounds.x + GetTextWidth(istate.buf)/2 + bounds.width/2 + 1;
            else if (textAlignment == GUI_TEXT_ALIGN_RIGHT) cursor.x = bounds.x + bounds.width - GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
        }
        else {
            if (CheckCollisionPointRec(mousePoint, bounds)) {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            }
        }

        if (pressed) istate.framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_PRESSED) {
        GuiDrawRectangle(bounds, 
            GuiGetStyle(TEXTBOX, BORDER_WIDTH), 
            Fade(GetColor(GuiGetStyle(TEXTBOX, (state*3))), 1.0f), 
            Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), 1.0f));

        // Draw blinking cursor
        if (istate.editing && ((istate.framesCounter/20)%2 == 0)) 
            GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), 1.0f));
    }
    else if (state == GUI_STATE_DISABLED) {
        GuiDrawRectangle(bounds, 
            GuiGetStyle(TEXTBOX, BORDER_WIDTH), 
            Fade(GetColor(GuiGetStyle(TEXTBOX, (state*3))), 1.0f), 
            Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), 1.0f));
    }
    else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, (state*3))), 1.0f), BLANK);

    GuiDrawText(
        istate.buf, 
        GetTextBounds(TEXTBOX, bounds), 
        GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), 
        Fade(GetColor(GuiGetStyle(TEXTBOX, 2 + (state*3))), 1.0f));

    val = std::string{istate.buf};
    
    if (pressed)
        istate.editing = !istate.editing;
}

void GuiStringInput(InputState& state, Rectangle reg, std::string& val, const int fontSize) {
    char buff[512] = {'\0'};
    sprintf(buff, "%s", val.c_str()); 
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        state.editing = CheckCollisionPointRec(GetMousePosition(), reg);
    GuiTextBox(reg, buff, fontSize, state.editing);
    val = std::string{buff};
} 

void GuiStringInputValidate(
    InputState& state, 
    Rectangle reg,
    std::string& val,
    const int fontSize, 
    std::function<bool(const std::string&)>& validator) {

}