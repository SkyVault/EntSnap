#ifndef ENTSNAP_ENTITY_HPP
#define ENTSNAP_ENTITY_HPP

#include "Component.hpp"

#include <map>
#include <string>

namespace EntSnap {
    class Ent {
    public:
        bool editingName{false};
        std::string name{""};
        std::vector<Component> components;

        bool has(const std::string& name);
    };
}

#endif //ENTSNAP_ENTITY_HPP
