#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include <string>
#include <ostream>
#include <istream>

enum class PlayerType
{
	Human,
	Computer,
};

class ConsoleParams
{
public:
	ConsoleParams(int argc, char* argv[]);

	bool IsHelpPrintEnabled() const { return m_vm.count("help") > 0; }

	void PrintHelp(std::ostream& out) const { out << m_desc; }
private:

	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;
};