#include <iostream>
#include "CGNSCopier.h"
#include "CGIOWrapper.h"

#include <argp.h>
const char *argp_program_version = "CGNS Converter 0.1";
const char *argp_program_bug_address = "ilya.pogorelov@phystech.edu";

struct TOptions
{
	std::string src_fname, src_lib = "system";
	std::string dst_fname, dst_lib = "system";
	bool showVersions = false;
};

static error_t parse_opt(int key, char *arg, argp_state *state)
{
	TOptions &options = *static_cast<TOptions*>(state->input);
	switch (key)
	{
		case 'i':
			options.src_lib = arg;
			break;
		case 'o':
			options.dst_lib = arg;
			break;
		case 's':
			options.showVersions = true;
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num == 0)
				options.src_fname = arg;
			else if (state->arg_num == 1)
				options.dst_fname = arg;
			else
				argp_usage(state);
			break;

		case ARGP_KEY_END:
			if (state->arg_num < 2)
				// Not enough args
				argp_usage(state);
			break;

		case ARGP_KEY_INIT:
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

TOptions parse_arguments(int argc, char **argv)
{
	TOptions args;

	argp parser{};
	parser.doc = "Converter between CGNS/HDF5 lib versions";
	parser.args_doc = "SRC.cgns DST.cgns";

	argp_option options[] = {
	    {"inputlib" , 'i', "DIRECTORY", 0, "Input file library dir or system (default)", 0},
	    {"outputlib", 'o', "DIRECTORY", 0, "Output file library dir or system (default)", 0},
	    {"showvers" , 's', nullptr,     0, "Show used libary versions", 0},
	    { nullptr }
	};
	parser.options = options;
	parser.parser = parse_opt;

	argp_parse(&parser, argc, argv, 0, nullptr, &args);

	return args;
}

int main(int argc, char **argv)
{
	CGIOWrapper cgi, cgo;
	CGNSCopier copier(cgi, cgo);
	bool ok = true;
	try
	{
		auto options = parse_arguments(argc, argv);

		cgi.load_libs(options.src_lib);
		cgo.load_libs(options.dst_lib);

		if (options.showVersions)
		{
			std::cout << "Input:  " << cgi.library_versions() << std::endl;
			std::cout << "Output: " << cgo.library_versions() << std::endl;
		}

		copier.open_in(options.src_fname);
		copier.open_out(options.dst_fname);

		copier.copy();
	}
	catch (std::string msg)
	{
		std::cerr << msg << std::endl;
		ok = false;
	}

	return ok ? 1 : 0;
}
