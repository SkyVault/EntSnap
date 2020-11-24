//
// Created by dustin on 11/19/20.
//

#ifndef ENTSNAP_COMPONENT_HPP
#define ENTSNAP_COMPONENT_HPP

#include <variant>
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

    static const std::map<std::string, Types> TypesM = {
        GENERATE_TYPES(GENERATE_PAIR)
    };

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
        char guiName[512] = {'\0'};

        bool guiValueEdit{false};
        char guiValue[512] = {'\0'};
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
        char name[512] = {'\0'};
        std::vector<GuiObject> props{};

        inline void reset() {
            std::memset(name, 0, sizeof(char)*512);
            props.clear();
        }

        inline Component toComponent() {
            std::vector<Object> props{};

            for (const auto& guiObj : this->props) {
                Value value;

                switch (static_cast<Types>(guiObj.type)) {
                    case Types::STRING: value = std::string{guiObj.guiValue}; break;
                    case Types::FLOAT: value = (double)std::stof(guiObj.guiValue); break;
                    case Types::INTEGER: value = (long)std::stod(guiObj.guiValue); break;
                    default:
                        std::cout << "Unhandled object type: " << guiObj.type << std::endl;
                }


                props.emplace_back(Object{
                    .type = guiObj.type,
                    .name = std::string{guiObj.guiName},
                    .value = std::move(value),
                });
            }

            return Component {
                .name = std::string{this->name},
                .props = std::move(props),
            };
        }
    };
}
#endif //ENTSNAP_COMPONENT_HPP
