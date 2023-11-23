#include "CGNSCopier.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cgnslib.h>

void CGNSCopier::cgio_check_error(int errCode, const CGIOWrapper &src)
{
	if (errCode != CGIO_ERR_NONE)
	{
		char err[CGIO_MAX_ERROR_LENGTH+1];
		src.error_message(err);
		std::string msg = "CGIO: "; msg += err;
		std::cout << msg << std::endl;
		throw msg;
	}
}

CGNSCopier::CGNSCopier(CGIOWrapper &cgi, CGIOWrapper &cgo)
    : cgi(cgi), cgo(cgo)
{}

CGNSCopier::~CGNSCopier() { close(); }

std::string CGNSCopier::getLibVersions() const
{
	std::string result;
	char hdfVersion[CGIO_MAX_VERSION_LENGTH+1];

	cgi.library_version(fin, hdfVersion);
	result += "Input: ";
	result += hdfVersion;
	result += '\n';

	cgo.library_version(fin, hdfVersion);
	result += "Output: ";
	result += hdfVersion;

	return result;
}


void CGNSCopier::open_in(const std::string &fname)
{
	int cgioRet = cgi.open_file(fname.c_str(), CGIO_MODE_READ, CGIO_FILE_HDF5, &fin);
	cgio_check_error(cgioRet, cgi);
}

void CGNSCopier::open_out(const std::string &fname)
{
	int cgioRet = cgo.open_file(fname.c_str(), CGIO_MODE_WRITE, CGIO_FILE_HDF5, &fout);
	cgio_check_error(cgioRet, cgo);
}

void CGNSCopier::close()
{
	if (fin  >= 0) cgio_check_error(cgi.close_file(fin ), cgi);
	if (fout >= 0) cgio_check_error(cgo.close_file(fout), cgo);
}

void CGNSCopier::copy()
{
	cgioid_t fin_root, fout_root;
	cgio_check_error(cgi.get_root_id(fin, &fin_root  ), cgi);
	cgio_check_error(cgo.get_root_id(fout, &fout_root), cgo);
	copyNode(fin_root, fout_root);
}

void CGNSCopier::copyNode(cgioid_t from, cgioid_t to)
{
	cgioid_t root_id;
	cgo.get_root_id(fout, &root_id);
	const std::string CGNSVersionLabel = "CGNSLibraryVersion_t";
	bool isVersion=false;
	if (to != root_id)
	{
		char label[CGIO_MAX_LABEL_LENGTH+1];
		cgio_check_error( cgi.get_label(fin, from, label), cgi );
		cgio_check_error( cgo.set_label(fout, to, label ), cgo );
		isVersion = CGNSVersionLabel == label;
	}

	int is_link;
	cgi.is_link(fin, to, &is_link);
	if (is_link) { return; }


	char dtype[CGIO_MAX_DATATYPE_LENGTH+1];
	int ndims;
	cgsize_t dims[12];
	cgio_check_error( cgi.get_data_type(fin, from, dtype), cgi );
	if (strcmp("MT", dtype))
	{
		cgio_check_error( cgi.get_dimensions(fin, from,        &ndims, dims), cgi );
		cgio_check_error( cgo.set_dimensions(fout,  to,  dtype, ndims, dims), cgo );

		cglong_t dsize;
		cgio_check_error( cgi.get_data_size(fin, from, &dsize), cgi );

		char *data = new char [dsize];
		cgio_check_error( cgi.read_all_data(fin, from, data), cgi );
		cgio_check_error( cgo.write_all_data(fout, to, data), cgo );
		delete [] data;

//		if (isVersion)
//		{
//			char newVersion[CGIO_MAX_VERSION_LENGTH+1];
//			cgio_check_error( cgo.library_version(fout, newVersion), cgo );
//			float v;
//			sscanf(newVersion, "%g", &v);
//			cgio_check_error( cgo.write_all_data(fout, to, &v), cgo );
//		}
	}


	int nchildren;
	cgio_check_error( cgi.number_children(fin, from, &nchildren), cgi );
	for (int c = 0; c < nchildren; ++c)
	{
		cgioid_t from_cid;
		int nret;
		cgio_check_error( cgi.children_ids(fin, from, c+1, 1, &nret, &from_cid), cgi );

		char name[CGIO_MAX_NAME_LENGTH+1];
		cgio_check_error( cgi.get_name(fin, from_cid, name), cgi );

		cgioid_t to_cid;
		cgio_check_error( cgo.create_node(fout, to, name, &to_cid), cgo );
		copyNode(from_cid, to_cid);
	}
}
