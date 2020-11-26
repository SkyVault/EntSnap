#include "App.hpp"

#include <json.hpp>
using json = nlohmann::json;

constexpr int INDENT {2};

void App::serializeProject(const std::filesystem::path& path) {
    json out;

    out["project-name"] = currentProj.name;

    auto serComp = [](const Component& c) -> json {
        json obj = json::object();

        obj["name"] = c.name;

        json props;
        for (auto& [type, name, value] : c.props) {
            json prop = json::object();
            prop["type"] = TypesS[type];
            prop["name"] = name;

            switch(static_cast<Types>(type)) {
                case Types::STRING: prop["value"] = std::get<std::string>(value); break;
                case Types::FLOAT: prop["value"] = std::get<double>(value); break;
                case Types::INTEGER: prop["value"] = std::get<long>(value); break;
                default:
                    std::cout
                        << "Serializer does not support property type: "
                        << TypesS[type]
                        << std::endl;
            }

            props.push_back(prop);
        }

        obj["props"] = props;
        return obj;
    };

    json compObjects;
    for (const auto& c : currentProj.components) {
        compObjects.push_back(serComp(c));
    }

    out["components"] = compObjects;

    json entities;
    for (const auto& e : currentProj.entities) {
        json entity = json::object();
        entity["name"] = e.name;

        json entComps;
        for (const auto& c : e.components) {
            entComps.push_back(serComp(c));
        }

        entity["components"] = entComps;
        entities.push_back(entity);
    }

    out["entities"] = entities;

    std::ofstream outFile(path);
    outFile << out.dump(INDENT);
    outFile.close();
}

void App::loadProject(const std::filesystem::path& path) {
    std::string data;
    std::ifstream inFile(path);

    std::string line;
    while (std::getline(inFile, line)) {
        data += line + "\n";
    }

    inFile.close();

    json in = json::parse(data);
    currentProj = Proj{};
    currentProj.name = in["project-name"];
    currentProj.path = path;

    auto readComp = [](json comp) -> Component {
        auto result = Component{};
        result.name = comp["name"];

        for (const json& prop : comp["props"]) {
            auto presult = Object{};
            const std::string t = prop["type"].get<std::string>();
            presult.type = static_cast<int>(TypesM[t]);
            presult.name = prop["name"];

            switch (static_cast<Types>(presult.type)) {
                case Types::INTEGER:
                    presult.value = prop["value"].get<long>();
                    break;
                case Types::FLOAT:
                    presult.value = prop["value"].get<double>();
                    break;
                case Types::STRING:
                    presult.value = prop["value"].get<std::string>();
                    break;
            }

            result.props.emplace_back(std::move(presult));
        }
        return result;
    };

    for (const auto& comp : in["components"]) {
        currentProj.components.emplace_back(readComp(comp));
    }

    for (const auto& ent : in["entities"]) {
        Ent resultEnt;
        resultEnt.name = ent["name"];

        for (const auto& comp : ent["components"]) {
            resultEnt.components.emplace_back(readComp(comp));
        }

        currentProj.entities.emplace_back(resultEnt);
        editingEntities.emplace_back(std::make_unique<Ent>(resultEnt));
    }
}
