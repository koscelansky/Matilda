#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include <string>
#include <ostream>

class ConsoleParams
{
public:
	ConsoleParams(int argc, char* argv[])
		: m_desc("Allowed options")
	{
		// declare supported options
		m_desc.add_options()
			("help,h", "print help message")
			("black,b", boost::program_options::value<std::string>(), "set black player")
			("white,w", boost::program_options::value<std::string>(), "set white player");

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_desc), m_vm);
		boost::program_options::notify(m_vm);
	}

	bool IsHelpPrintEnabled() const { return m_vm.count("help") > 0; }

	void PrintHelp(std::ostream& out) const { out << m_desc; }
private:

	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;
};