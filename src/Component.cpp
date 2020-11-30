#include "Component.hpp"

EntSnap::Component EntSnap::ToComponent(const EntSnap::GuiComponent& in) {
    std::vector<Object> props{};

    for (const auto& guiObj : in.props) { 
        props.emplace_back(Object{
            .type = guiObj.type,
            .name = std::string{guiObj.guiName},
            .value = std::string{guiObj.guiValue},
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
		sprintf(resultProp.guiValue, "%s", inProp.value.c_str()); 
        result.props.emplace_back(resultProp);
    }

    return result;
}
