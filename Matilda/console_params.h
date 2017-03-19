#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include "slovak_checkers/game.h"

#include <string>
#include <ostream>
#include <istream>

class ConsoleParams
{
public:
	ConsoleParams(int argc, char* argv[]);

	bool IsHelpPrintEnabled() const { return m_vm.count("help") > 0; }

	sc::PlayerType GetWhitePlayer() const { return m_vm["white"].as<sc::PlayerType>(); }

	sc::PlayerType GetBlackPlayer() const { return m_vm["black"].as<sc::PlayerType>(); }

	void PrintHelp(std::ostream& out) const { out << m_desc; }
private:

	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;
};