#pragma once

#include "openvic-simulation/interface/GFXObject.hpp"
#include "openvic-simulation/interface/GUI.hpp"

namespace OpenVic {

	class UIManager {
		NamedInstanceRegistry<GFX::Sprite> IDENTIFIER_REGISTRY(sprite);
		IdentifierRegistry<GFX::Font> IDENTIFIER_REGISTRY(font);
		NamedInstanceRegistry<GFX::Object> IDENTIFIER_REGISTRY(object);

		NamedInstanceRegistry<GUI::Scene, UIManager const&> IDENTIFIER_REGISTRY(scene);

		bool _load_font(ast::NodeCPtr node);
		NodeTools::NodeCallback auto _load_fonts(std::string_view font_key);

	public:
		bool add_font(
			std::string_view identifier, colour_argb_t colour, std::string_view fontname, std::string_view charset,
			uint32_t height
		);

		void lock_gfx_registries();

		bool load_gfx_file(ast::NodeCPtr root);
		bool load_gui_file(std::string_view scene_name, ast::NodeCPtr root);
	};
}
