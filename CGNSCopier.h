#pragma once
#include <string>
#include "CGIOWrapper.h"

class CGNSCopier
{
	static constexpr int cgns_max_name_length = 33;
	int fin = -1, fout = -1;

public:
	explicit CGNSCopier(CGIOWrapper &cgi, CGIOWrapper &cgo);
	~CGNSCopier();

	std::string getLibVersions() const;

	void open_in (const std::string &fname);
	void open_out(const std::string &fname);
	void close();

	void copy();


private:
	CGIOWrapper cgi, cgo;

	void copyNode(cgioid_t from, cgioid_t to);
	void cgio_check_error(int errCode, const CGIOWrapper &src);
};

