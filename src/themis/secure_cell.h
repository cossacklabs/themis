/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _SECURE_CELL_H_
#define _SECURE_CELL_H_

typedef struct themis_file_type themis_file_t;

struct themis_file_interface_type{
  FILE* file;
  char* filename;
  uint8_t mode;
  FILE (*fopen)( const char * filename, const char * mode );
  size_t (*fread)( void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
  size_t (*fwrite)( const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
  int (*fclose)( FILE *fp );
};

typedef struct themis_file_interface_type themis_file_interface_t;

struct themis_keydb_interface_type{
  themis_file_interface_t* db;
  themis_status_t (* get_key)(themis_file_interface_t* db, const uint8_t* id, const size id_length, uint8_t* key, size_t* key_size);
 
};


typedef struct themis_keydb_interface_type themis_keydb_interface_t;

themis_file_t* themis_file_open(const uint8_t* master_key,
				const size_t master_key_length,
				themis_file_interface_t* file,
				themis_keydb_interface_t* key_db);

themis_status_t themis_file_read(themis_file_t* file, uint8_t* buffer, size_t* buffer_length);

themis_ststus_t themis_file_write(themis_file_t* file, const uint8_t* buffer, const size_t buffer_length);

themis_status_t themis_file_close(themis_file_t* file);


#endif /* _SECURE_CELL_H_ */

















