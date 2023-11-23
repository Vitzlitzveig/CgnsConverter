#pragma once

#include <dlfcn.h>
#include <string>

#include <cgns_io.h>
#include <cgnslib.h>
//#include <hdf>

using cgioid_t = double;
#ifdef cgsize_t
#undef cgsize_t
using cgsize_t = cglong_t;
#endif

class CGIOWrapper
{
public:

	explicit CGIOWrapper();
	~CGIOWrapper();
	void load_libs(std::string libdir = "system");
	void close();
	std::string library_versions();


	// Common funcs
	typedef int (*f_open_file)       (const char *filename, int mode, int file_type, int *file_id);
	typedef int (*f_close_file)      (int cgio_num);
	typedef int (*f_error_message)   (char *error_msg);
	typedef int (*f_library_version) (int cgio_num, char *version);
	typedef int (*f_hdf5_libver)  (unsigned *majnum, unsigned *minnum, unsigned *relnum);

	f_open_file open_file;
	f_close_file close_file;
	f_error_message error_message;
	f_library_version library_version;
	f_hdf5_libver hdf5_libversion;

	// Reading
	typedef int (*f_get_root_id)    (int cgio_num, cgioid_t *rootid);
	typedef int (*f_get_label)      (int cgio_num, cgioid_t id, char *label);
	typedef int (*f_get_name)       (int cgio_num, cgioid_t id, char *name);
	typedef int (*f_is_link)        (int cgio_num, cgioid_t id, int *link_len);
	typedef int (*f_link_size)      (int cgio_num, cgioid_t id, int *file_len, int *name_len);
	typedef int (*f_get_link)       (int cgio_num, cgioid_t id, char *filename, char *name_in_file);
	typedef int (*f_number_children)(int cgio_num, cgioid_t id, int *num_children);
	typedef int (*f_children_ids)   (int cgio_num, cgioid_t id, int start, int max_ret, int *num_ret, cgioid_t *ids);
	typedef int (*f_get_data_size)  (int cgio_num, cgioid_t id, cglong_t *data_size);
	typedef int (*f_get_data_type)  (int cgio_num, cgioid_t id, char *data_type);
	typedef int (*f_get_dimensions) (int cgio_num, cgioid_t id, int *num_dims, cgsize_t *dims);
	typedef int (*f_read_all_data)  (int cgio_num, cgioid_t id, void *data);
	typedef int (*f_read_all_data_type) (int cgio_num, cgioid_t id, const char *m_data_type, void *data);

	f_get_root_id get_root_id;
	f_get_label get_label;
	f_get_name get_name;
	f_is_link is_link;
	f_link_size link_size;
	f_get_link get_link;
	f_number_children number_children;
	f_children_ids children_ids;
	f_get_data_size get_data_size;
	f_get_data_type get_data_type;
	f_get_dimensions get_dimensions;
	int read_all_data(int cgio_num, cgioid_t id, void *data);

	// Writing
	typedef int (*f_create_node)(int cgio_num, cgioid_t pid, const char *name, cgioid_t *id);
	typedef int (*f_create_link)(int cgio_num, cgioid_t pid, const char *name,
	                             const char *filename, const char *name_in_file, cgioid_t *id);
	typedef int (*f_set_name)(int cgio_num, cgioid_t pid, cgioid_t id, const char *name);
	typedef int (*f_set_label)(int cgio_num, cgioid_t id, const char *label);
	typedef int (*f_set_dimensions)(int cgio_num, cgioid_t id, const char *data_type,
	                                int num_dims,const cgsize_t *dims);
	typedef int (*f_write_all_data)(int cgio_num, cgioid_t id, const void *data);

	f_create_node create_node;
	f_create_link create_link;
	f_set_name set_name;
	f_set_label set_label;
	f_set_dimensions set_dimensions;
	f_write_all_data write_all_data;

private:
	Lmid_t context;
	void *libcgns = nullptr;
	void *libhdf5 = nullptr;

	template<typename T>
	void load_func(T &f, const char *fname);
	void load_read_data();


	const char *system_dl_path = "/usr/lib/x86_64-linux-gnu/";

	struct
	{
		// Common
		const char *cgio_open_file  = "cgio_open_file";
		const char *cgio_close_file = "cgio_close_file";
		const char *cgio_error_message = "cgio_error_message";
		const char *cgio_library_version = "cgio_library_version";
		const char *hdf5_libversion = "H5get_libversion";

		// Reading
		const char *cgio_get_root_id = "cgio_get_root_id";
		const char *cgio_get_label   = "cgio_get_label";
		const char *cgio_get_name    = "cgio_get_name";
		const char *cgio_is_link     = "cgio_is_link";
		const char *cgio_get_link    = "cgio_get_link";
		const char *cgio_get_link_size   = "cgio_link_size";
		const char *cgio_number_children = "cgio_number_children";
		const char *cgio_children_ids    = "cgio_children_ids";
		const char *cgio_get_data_size   = "cgio_get_data_size";
		const char *cgio_get_data_type   = "cgio_get_data_type";
		const char *cgio_get_dimensions  = "cgio_get_dimensions";
		const char *cgio_read_all_data   = "cgio_read_all_data";
		const char *cgio_read_all_data_type   = "cgio_read_all_data_type";

		// Writing
		const char *cgio_create_node    = "cgio_create_node";
		const char *cgio_create_link    = "cgio_create_link";
		const char *cgio_set_name       = "cgio_set_name";
		const char *cgio_set_label      = "cgio_set_label";
		const char *cgio_set_dimensions = "cgio_set_dimensions";
		const char *cgio_write_all_data = "cgio_write_all_data";

	} names;

	f_read_all_data read_all_data_ref;
	f_read_all_data_type read_all_data_type;

};


