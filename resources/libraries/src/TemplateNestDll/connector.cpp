#include "TemplateNestdll.h"

#include "TemplateNestClass.h"


	defvaltype convert_text(const char* text,string & error);


	defvaltype convert_jsontext(const char* text, string& error);



	  void templatenest_init(void** object) {
	//TemmplNest
	*object =(void*) new TemplateNestClass();

}

	/*  has Str $.template_dir is rw;
	  has Str $.template_ext is rw = '.html';
	  has% .template_hash is rw;
	  has% .defaults is rw;

	  subset Char of Str where.chars == 1 || .chars == 0;

	  has Char $.defaults_namespace_char is rw = '.';

	  my @comment_delims_defaults[2] = '<!--', '-->';
	  has Str @.comment_delims[2] is rw;

	  my @token_delims_defaults[2] = '<%', '%>';
	  has Str @.token_delims[2] is rw;

	  has Bool $.show_labels is rw = False;
	  has Str $.name_label is rw = 'TEMPLATE';
	  has Bool $.fixed_indent is rw = False;
	  has Bool $.die_on_bad_params is rw = False;
	  has Char $.escape_char is rw = '\\';

	  has Pointer $class_pointer = Pointer.new();*/
	
	  void templatenest_set_parameters(void* object, char * defaults, char * template_dir,char * template_ext, char* template_hash, char * defaults_namespace_char,char** comment_delims,
		   char** token_delims,int64_t show_labels, char * name_label, int64_t fixed_indent, int64_t die_on_bad_params, char * escape_char,  int64_t preindex,int64_t indexes)
		  
	  {
	  TemplateNestClass & c = *((TemplateNestClass*)object);
	  string error;
	  defvaltype v = convert_text(defaults, error);
	  ((TemplateNestClass*)object)->conv_error = error;
	  c.defaults = v;

	  v = convert_text(template_hash, error);
	  ((TemplateNestClass*)object)->conv_error = error;
	  c.template_hash = v;

	  c.template_dir = template_dir;
	  c.template_ext = template_ext;
	  c.defaults_namespace_char = defaults_namespace_char;

	  c.comment_delims[0] = comment_delims[0];
	  c.comment_delims[1] = comment_delims[1];
	  c.token_delims[0] = token_delims[0];
	  c.token_delims[1] = token_delims[1];
	  c.show_labels = show_labels;
	  c.name_label = name_label;
	  c.fixed_indent = fixed_indent;
	  c.die_on_bad_params = die_on_bad_params;
	  c.escape_char = escape_char;
	  c.indexes = indexes;
	  if (preindex)
	  {
		  ((TemplateNestClass*)object)->make_index();
	  }


	  }

	  /*void templatenest_make_index(void* object)
	  {
		  TemplateNestClass& c = *((TemplateNestClass*)object);
		  c.make_index();


	  }*/


	  void templatenest_set_jsonparameters(void* object, char* defaults, char* template_dir, char* template_ext, char * template_hash,char* defaults_namespace_char, char** comment_delims,
		  char** token_delims, int64_t show_labels, char* name_label, int64_t fixed_indent, int64_t die_on_bad_params, char* escape_char,  int64_t preindex,int64_t indexes)

	  {
		 
		  TemplateNestClass& c = *((TemplateNestClass*)object);
		  string error;
		 
		  defvaltype v = convert_jsontext(defaults, error);
		  ((TemplateNestClass*)object)->conv_error = error;
		  c.defaults = v;

		  v = convert_jsontext(template_hash, error);
		  ((TemplateNestClass*)object)->conv_error = error;
		  c.template_hash = v;


		  c.template_dir = template_dir;
		 
		  c.template_ext = template_ext;
		  
		  c.defaults_namespace_char = defaults_namespace_char;

		  c.comment_delims[0] = comment_delims[0];
		 
		  c.comment_delims[1] = comment_delims[1];
		  
		  c.token_delims[0] = token_delims[0];
		  
		  c.token_delims[1] = token_delims[1];
		 
		  c.show_labels = show_labels;
		  c.name_label = name_label;
		  c.fixed_indent = fixed_indent;
		  c.die_on_bad_params = die_on_bad_params;
		  c.escape_char = escape_char;
		  c.indexes = indexes;
		 
		  if (preindex)
		  {
			  ((TemplateNestClass*)object)->make_index();
		  }

	  }
	  
void templatenest_render(void* object,char* data,char ** output,char ** err)
{
	string error;
	
	defvaltype v = convert_text(data, error);
	((TemplateNestClass*)object)->conv_error = error;
	*err = (char*)((TemplateNestClass*)object)->conv_error.c_str();
	if (string(*err) != "")
	{
		*output = (char *)"";
		return;
	}
	

	((TemplateNestClass*)object)->output =   ((TemplateNestClass*)object)->rendertop(v);
	*err = (char*)((TemplateNestClass*)object)->die.c_str();
	*output = (char*)((TemplateNestClass*)object)->output.c_str();
}


void templatenest_makeindex(void* object)
{
	((TemplateNestClass*)object)->make_index();
}


void templatenest_makeindexbyname(void* object,char * name)
{
	((TemplateNestClass*)object)->make_index(name);

}


void templatenest_jsonrender(void* object, char* data, char** output, char** err)
{
	string error;
	defvaltype v = convert_jsontext(data, error);
	((TemplateNestClass*)object)->conv_error = error;
	*err = (char*)((TemplateNestClass*)object)->conv_error.c_str();
	if (string(*err) != "")
	{
		*output = (char*)"";
		return;
	}
	
	((TemplateNestClass*)object)->output = ((TemplateNestClass*)object)->rendertop(v);
	*err = ( char*)"";
	*output = (char*)"";
	*err = (char*)((TemplateNestClass*)object)->die.c_str();
	*output = (char*)((TemplateNestClass*)object)->output.c_str();
}

void templatenest_destroy(void* object)
{
	delete (TemplateNestClass*)object;
}

void get_error(void* object, char** text)
{
	*text = (char*)((TemplateNestClass*)object)->died().c_str();

	//*text = (char*)"somethintg";
}
