#pragma once

#include "openvic-simulation/scripts/Condition.hpp"
#include "openvic-simulation/scripts/Script.hpp"

namespace OpenVic {
	struct DefinitionManager;

	struct ConditionScript final : Script<DefinitionManager const&> {

	private:
		ConditionNode PROPERTY_REF(condition_root);
		scope_t PROPERTY(initial_scope);
		scope_t PROPERTY(this_scope);
		scope_t PROPERTY(from_scope);

	protected:
		bool _parse_script(ast::NodeCPtr root, DefinitionManager const& definition_manager) override;

	public:
		ConditionScript(scope_t new_initial_scope, scope_t new_this_scope, scope_t new_from_scope);
	};
}
