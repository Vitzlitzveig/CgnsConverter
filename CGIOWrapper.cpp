#include "CGIOWrapper.h"
#include <sstream>
#include <iostream>

CGIOWrapper::CGIOWrapper() {}

CGIOWrapper::~CGIOWrapper()
{
	close();
}

void CGIOWrapper::load_libs(std::string libdir)
{
	if (libdir == "system")
		libdir = system_dl_path;

	if (libdir.size() && libdir[libdir.size()-1]!='/')
		libdir += '/';

	std::string lcgns = libdir + "libcgns.so";
	std::string lhdf5 = libdir + "libhdf5.so";

	libhdf5 = dlmopen(LM_ID_NEWLM, lhdf5.c_str(), RTLD_NOW);
	if (!libhdf5)
		throw std::string(dlerror());

	dlinfo(libhdf5, RTLD_DI_LMID, &context);
	libcgns = dlmopen(context, lcgns.c_str(), RTLD_LAZY);
	if (!libcgns)
		throw std::string(dlerror());
	dlerror();

	load_func(open_file, names.cgio_open_file);
	load_func(close_file, names.cgio_close_file);
	load_func(error_message, names.cgio_error_message);
	load_func(library_version, names.cgio_library_version);

	load_func(get_root_id, names.cgio_get_root_id);
	load_func(get_label, names.cgio_get_label);
	load_func(get_name, names.cgio_get_name);
	load_func(is_link, names.cgio_is_link);
	load_func(get_link, names.cgio_get_link);
	load_func(link_size, names.cgio_get_link_size);
	load_func(number_children, names.cgio_number_children);
	load_func(children_ids, names.cgio_children_ids);
	load_func(get_data_size, names.cgio_get_data_size);
	load_func(get_data_type, names.cgio_get_data_type);
	load_func(get_dimensions, names.cgio_get_dimensions);
	load_read_data();

	load_func(create_node, names.cgio_create_node);
	load_func(create_link, names.cgio_create_link);
	load_func(set_name, names.cgio_set_name);
	load_func(set_label, names.cgio_set_label);
	load_func(set_dimensions, names.cgio_set_dimensions);
	load_func(write_all_data, names.cgio_write_all_data);

	hdf5_libversion = (f_hdf5_libver) dlsym(libhdf5, names.hdf5_libversion);
}

void CGIOWrapper::close()
{
	if (libcgns && dlclose(libcgns))
		std::cerr << dlerror();
	if (libhdf5 && dlclose(libhdf5))
		std::cerr << dlerror();
}

std::string CGIOWrapper::library_versions()
{
	std::ostringstream stream;

	unsigned maj, min, minimin;

	int cgver= *(int *) dlsym(libcgns, "CGNSLibVersion");

	maj = cgver/1000;
	min = (cgver/100)%10;
	minimin = cgver % 100;
	stream << "CGNS " << maj << '.' << min << '.' << minimin << "; ";

	hdf5_libversion(&maj, &min, &minimin);
	stream << "HDF5 " << maj << '.' << min << '.' << minimin;

	return stream.str();
}

template <typename T>
void CGIOWrapper::load_func(T &f, const char *fname)
{
	f = nullptr;
	f = T(dlsym(libcgns, fname));
	if (!f)
		throw std::string(dlerror());
}

void CGIOWrapper::load_read_data()
{
	read_all_data_ref  = (f_read_all_data     ) dlsym(libcgns, names.cgio_read_all_data     );
	read_all_data_type = (f_read_all_data_type) dlsym(libcgns, names.cgio_read_all_data_type);

	if (read_all_data_ref || read_all_data_type)
		return;

	throw std::string(dlerror());
}

int CGIOWrapper::read_all_data(int cgio_num, cgioid_t id, void *data)
{
	if (read_all_data_ref)
		return read_all_data_ref(cgio_num, id, data);

	int ret;
	char type[CGIO_MAX_DATATYPE_LENGTH+1];
	ret = get_data_type(cgio_num, id, type);
	if (ret != CGIO_ERR_NONE) return ret;
	return read_all_data_type(cgio_num, id, type, data);
}

