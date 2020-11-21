#ifndef ENTSNAP_PROJECT_HPP
#define ENTSNAP_PROJECT_HPP

#include <vector>

#include "Component.hpp"
#include "Entity.hpp"

namespace EntSnap {
    class Proj {
    public:
        std::vector<Component> components;
        std::vector<Ent> entities;
    };
}

#endif //ENTSNAP_PROJECT_HPP
