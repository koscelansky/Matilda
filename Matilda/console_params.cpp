#include "console_params.h"

namespace po = boost::program_options;

ConsoleParams::ConsoleParams(int argc, char * argv[])
	: m_desc("Allowed options")
{
	// declare supported options
	po::positional_options_description p;
	p.add("fen", 1);

	m_desc.add_options()
		("help,H", po::bool_switch(), "print help message")
		("fen", po::value<std::string>(), "game state in FEN format");
	
	po::store(po::command_line_parser(argc, argv).options(m_desc).positional(p).run(), m_vm);
	po::notify(m_vm);
}
