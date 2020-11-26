#include "Entity.hpp"

bool EntSnap::Ent::has(const std::string& name) {
    for (const auto& c : components) {
        if (c.name == name) return true;
    }
    return false;
}
