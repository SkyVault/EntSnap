#include "App.hpp"

#include <json.hpp>
using json = nlohmann::json;

void App::serializeProject(const std::filesystem::path& path) {
    json out;

    out["project-name"] = currentProj.name;

    json compObjects;

    for (const auto& c : currentProj.components) {
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

        compObjects.push_back(obj);
    }

    out["components"] = compObjects;

    std::ofstream outFile(path);
    outFile << out.dump();
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

    for (const auto& comp : in["components"]) {

    }

    std::cout << in.dump() << std::endl;
}
