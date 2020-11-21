#ifndef ENTSNAP_ENTITY_HPP
#define ENTSNAP_ENTITY_HPP

#include "Component.hpp"

#include <map>
#include <string>

namespace EntSnap {
    class Ent {
        std::string name{""};
        std::map<std::string, Component> components;
    };
}

#endif //ENTSNAP_ENTITY_HPP
