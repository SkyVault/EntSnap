//
// Created by dustin on 11/19/20.
//

#ifndef ENTSNAP_COMPONENT_HPP
#define ENTSNAP_COMPONENT_HPP

#include <variant>
#include <type_traits>
#include <string>
#include <cstring>
#include <tuple>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

namespace EntSnap {
    #define GENERATE_ENUM(ENUM) ENUM,
    #define GENERATE_STRING(STRING) #STRING,
    #define GENERATE_PAIR(VALUE) {#VALUE, Types::VALUE},

    #define GENERATE_TYPES(X)\
        X(STRING) \
        X(FLOAT) \
        X(INTEGER)

    enum class Types : int {
        GENERATE_TYPES(GENERATE_ENUM)
    };

    static const std::string TypesS[] = {
        GENERATE_TYPES(GENERATE_STRING)
    };

    static std::map<std::string, Types> TypesM = {
        GENERATE_TYPES(GENERATE_PAIR)
    };

    #define BUFF_SIZE (512)

    using Value = std::variant<std::string, double, long>;

    struct Object {
        int type;
        std::string name{""};
        Value value;
    };

    class GuiObject {
    public:
        // Gui stuff
        int type{0};
        bool guiTypeDropdown{false};

        bool guiNameEdit{false};
        char guiName[BUFF_SIZE] = {'\0'};

        bool guiValueEdit{false};
        char guiValue[BUFF_SIZE] = {'\0'};

        bool shouldRemove{false};
    };

    class Component {
    public:
        std::string name {""};
        std::vector<Object> props{};

        bool shouldRemoveFromList {false};
        bool shouldRemoveFromEnt {false};
    };

    class GuiComponent {
    public:
        char name[BUFF_SIZE] = {'\0'};
        std::vector<GuiObject> props{};

        inline void reset() {
            std::memset(name, 0, sizeof(char)*BUFF_SIZE);
            props.clear();
        }
    };

    Component ToComponent(const GuiComponent& in);
    GuiComponent ToGuiComponent(const Component& in);
}
#endif //ENTSNAP_COMPONENT_HPP
