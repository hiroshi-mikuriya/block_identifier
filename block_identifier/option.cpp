#pragma once

#include "option.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

Option getDefaultOption()
{
	Option opt;
	opt.colors = {
		{ "red", { 0x80, 0x80, 0xFF } },
		{ "green", { 0x80, 0xFF, 0x80 } },
		{ "white", { 0xFC, 0xFC, 0xFC } },
		{ "blue", { 0xFF, 0x80, 0x80 } },
		{ "aqua", { 0xFF, 0xE0, 0xC0 } },
		{ "yellow", { 0x80, 0xFF, 0xFF } },
	};
	opt.clr2inst = std::map<std::string, std::string>{
		{ "red", "object-ripple" },
		{ "green", "object-mario-run-anime" },
		{ "white", "object-mario-runandjump-anime 	" },
		{ "blue", "object-cube" },
		{ "aqua", "object-repbang" },
		{ "yellow", "object-fireworks" },
	};
	return opt;
}
