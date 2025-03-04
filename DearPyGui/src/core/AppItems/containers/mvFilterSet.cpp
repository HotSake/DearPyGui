#include "mvFilterSet.h"
#include "mvContext.h"
#include "mvItemRegistry.h"

namespace Marvel {

	void mvFilterSet::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{
		std::vector<mvPythonDataElement> args;

		AddCommonArgs(args,(CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_WIDTH |
			MV_PARSER_ARG_INDENT |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_BEFORE |
			MV_PARSER_ARG_SEARCH_DELAY |
			MV_PARSER_ARG_SHOW)
		);

		mvPythonParserSetup setup;
		setup.about = "Helper to parse and apply text filters (e.g. aaaaa[, bbbbb][, ccccc])";
		setup.category = { "Containers", "Widgets" };
		setup.returnType = mvPyDataType::UUID;
		setup.createContextManager = true;

		mvPythonParser parser = FinalizeParser(setup, args);

		parsers->insert({ s_command, parser });
	}

	mvFilterSet::mvFilterSet(mvUUID uuid)
		: mvAppItem(uuid)
	{
	}

	PyObject* mvFilterSet::getPyValue()
	{
		return ToPyString(std::string(_imguiFilter.InputBuf));
	}

	void mvFilterSet::setPyValue(PyObject* value)
	{
		auto str_value = ToString(value);

		int i = 0;
		for (auto& character : str_value)
		{
			if (i > 254)
				break;

			_imguiFilter.InputBuf[i] = character;

			++i;
		}
		_imguiFilter.InputBuf[i] = 0;
		_imguiFilter.Build();
	}

	void mvFilterSet::draw(ImDrawList* drawlist, float x, float y)
	{
		ScopedID id(_uuid);

		if (_width != 0)
			ImGui::PushItemWidth((float)_width);

		if (_imguiFilter.IsActive())
		{
			for (auto& childset : _children)
			{
				for (auto& child : childset)
				{
					if (!_imguiFilter.PassFilter(child->_filter.c_str()))
						continue;

					child->draw(drawlist, ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
				}
			}

		}
		else
		{

			for (auto& childset : _children)
			{
				for (auto& child : childset)
					child->draw(drawlist, ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
			}
		}

		if (_width != 0)
			ImGui::PopItemWidth();
	}

}