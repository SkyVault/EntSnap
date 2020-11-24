#ifndef ENTSNAP_PROJECT_HPP
#define ENTSNAP_PROJECT_HPP

#include <vector>
#include <filesystem>

#include "Component.hpp"
#include "Entity.hpp"

namespace EntSnap {
    class Proj {
    public:
        std::filesystem::path path{};
        std::string name{""};
        std::vector<Component> components;
        std::vector<Ent> entities;
    };
}

#endif //ENTSNAP_PROJECT_HPP
