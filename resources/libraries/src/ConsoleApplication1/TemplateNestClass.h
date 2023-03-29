#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
using namespace std;



struct defvaltype
{

    std::shared_ptr<unordered_map<string, defvaltype>> level;
    std::shared_ptr < vector<defvaltype>> array;
    string val;

    defvaltype()
    {
        level = nullptr;
        array = nullptr;
    }
    /*~defvaltype()
    {
        if (level != nullptr)
            delete level;
        if (array != nullptr)
            delete array;
    }*/
};

class TemplateNestClass
{
    public:
    //has Str $.template_dir is rw;
    string template_dir;
    //has Str $.template_ext is rw = '.html';
    string template_ext = ".html";
    //has% .template_hash is rw;
    defvaltype template_hash;

    //has% .defaults is rw;
    defvaltype defaults;
    //subset Char of Str where.chars == 1 || .chars == 0;

    //has Char $.defaults_namespace_char is rw = '.';
    string defaults_namespace_char = ".";
    string comment_delims_defaults[2] = { "<!--", "-->"};
    //my @comment_delims_defaults[2] = '<!--', '-->';
    //has Str @.comment_delims[2] is rw;
    string comment_delims[2];

    //my @token_delims_defaults[2] = '<%', '%>';
    string token_delims_defaults[2] = { "<%", "%>" };
    //has Str @.token_delims[2] is rw;
    string token_delims[2];

    //has Bool $.show_labels is rw = False;
    bool show_labels = false;
    //has Str $.name_label is rw = 'TEMPLATE';
    string name_label = "TEMPLATE";
    //has Bool $.fixed_indent is rw = False;
    bool fixed_indent = false;
    //has Bool $.die_on_bad_params is rw = False;
    bool die_on_bad_params = false;
    //has Char $.escape_char is rw = '\\';
    string escape_char  = "\\";


    

    bool indexes = false;
    string die = "";

    string conv_error = "";
    string output = "";
    struct param_locations_type
    {
        size_t p0;
        size_t p;
        string name;

    };
    struct location_info {

        string escape_char;
        vector<vector<param_locations_type> > loc;
    };
    unordered_map<string, location_info > param_locations;
   

    string rendertop(const defvaltype& comp);

    string render(const defvaltype & comp);
    string render_hash(unordered_map<string, defvaltype> h);
    string render_array(vector<defvaltype> arr);


    string get_template(string template_name);
    string fill_in(const string & template_name, const string& template1, const unordered_map<string, string> & params);
    vector<string> params(string template_name);
    vector<string> params_in(string text);
    
    string get_default_val(const  defvaltype& def, vector<string> parts);
    bool token_regex(string& param_name, const string& text, size_t& p0, size_t& p, bool fixed_pos, bool& found);
    string & died();
   
    
};

