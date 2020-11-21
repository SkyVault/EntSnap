//
// Created by dustin on 11/19/20.
//

#ifndef ENTSNAP_COMPONENT_HPP
#define ENTSNAP_COMPONENT_HPP

#include <variant>
#include <string>
#include <tuple>
#include <map>
#include <vector>

namespace EntSnap {
    enum class Types : int {
        STRING,
        FLOAT,
        INTEGER,
    };

    struct Object {
        int type;
        std::string name{""};
        std::variant<std::string, double, int> value;

        // Gui stuff
        bool guiTypeDropdown{false};

        bool guiNameEdit{false};
        char guiName[512];

        bool guiValueEdit{false};
        char guiValue[512];

    };

    class Component {
    public:
        std::string name {""};
        std::vector<std::tuple<std::string, Object>> props{};
    };
}
#endif //ENTSNAP_COMPONENT_HPP
