#include "Component.hpp"

EntSnap::Component EntSnap::ToComponent(const EntSnap::GuiComponent& in) {
    std::vector<Object> props{};

    for (const auto& guiObj : in.props) {
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
        .name = std::string{in.name},
        .props = std::move(props)
    };
}

EntSnap::GuiComponent EntSnap::ToGuiComponent(const EntSnap::Component& in) {
    GuiComponent result;
    sprintf(result.name, "%s", in.name.c_str());

    for (const auto& inProp : in.props) {
        GuiObject resultProp;
        resultProp.type = inProp.type;
        sprintf(resultProp.guiName, "%s", inProp.name.c_str());

        switch (static_cast<EntSnap::Types>(inProp.type)) {
            case EntSnap::Types::INTEGER: {
                sprintf(resultProp.guiValue, "%d", std::get<long>(inProp.value));
                break;
            }

            case EntSnap::Types::FLOAT: {
                sprintf(resultProp.guiValue, "%f", std::get<double>(inProp.value));
                break;
            }

            case EntSnap::Types::STRING: {
                sprintf(resultProp.guiValue, "%s", std::get<std::string>(inProp.value).c_str());
                break;
            }
        }

        result.props.emplace_back(resultProp);
    }

    return result;
}
