#pragma once
#define TEMPLATENEST_EXPORTS

#if defined (_WIN32) 

#ifdef TEMPLATENEST_EXPORTS
#define TEMPLATENEST_API __declspec(dllexport)
#else
#define TEMPLATENEST_API __declspec(dllimport)
#endif
#else
#define TEMPLATENEST_API
#endif

#include <cstdint>

extern  "C"  TEMPLATENEST_API  void templatenest_init(void * * object);


//extern  "C"  TEMPLATENEST_API void templatenest_make_index(void* object);


extern  "C"  TEMPLATENEST_API  void templatenest_set_parameters(void* object, char* defaults, char* template_dir, char* template_ext, char* template_hash,char* defaults_namespace_char, char** comment_delims,
	char** token_delims, int64_t show_labels, char* name_label, int64_t fixed_indent, int64_t die_on_bad_params, char* escape_char,  int64_t preindex, int64_t indexes);

extern  "C"  TEMPLATENEST_API  void templatenest_set_jsonparameters(void* object, char* defaults, char* template_dir, char* template_ext, char* template_hash, char* defaults_namespace_char, char** comment_delims,
	char** token_delims, int64_t show_labels, char* name_label, int64_t fixed_indent, int64_t die_on_bad_params, char* escape_char,  int64_t preindex, int64_t indexes);

extern "C" TEMPLATENEST_API void templatenest_render(void* object, char* data, char** output, char** err);
extern "C" TEMPLATENEST_API void templatenest_jsonrender(void* object, char* data, char** output, char** err);
extern "C" TEMPLATENEST_API void templatenest_destroy(void* object);
extern "C" TEMPLATENEST_API void get_error(void* object,  char** text);
