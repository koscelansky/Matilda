#include "console_params.h"

namespace po = boost::program_options;

void validate(boost::any& v, std::vector<std::string> const& values, PlayerType*, int)
{
	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	std::string const& s = po::validators::get_single_string(values);

	if (s == "human")
	{
		v = boost::any(PlayerType::Human);
	}
	else if (s == "computer")
	{
		v = boost::any(PlayerType::Computer);
	}
	else
	{
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}


ConsoleParams::ConsoleParams(int argc, char * argv[])
	: m_desc("Allowed options")
{
	// declare supported options
	m_desc.add_options()
		("help,h", "print help message")
		("white,w", po::value<PlayerType>()->default_value(PlayerType::Human, "human"), "set white player")
		("black,b", po::value<PlayerType>()->default_value(PlayerType::Computer, "computer"), "set black player");

	po::store(po::parse_command_line(argc, argv, m_desc), m_vm);
	po::notify(m_vm);
}
