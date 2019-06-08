#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include <string>
#include <ostream>
#include <istream>

class ConsoleParams
{
public:
	ConsoleParams(int argc, char* argv[]);

	bool IsHelpPrintEnabled() const { return m_vm.count("help") > 0; }

	bool HasFen() const { return m_vm.count("fen") > 0; }

	std::string GetFen() const { return m_vm["fen"].as<std::string>(); }

	void PrintHelp(std::ostream& out) const { out << m_desc; }
private:

	boost::program_options::options_description m_desc;
	boost::program_options::variables_map m_vm;
};