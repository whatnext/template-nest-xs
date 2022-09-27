#include "TemplateNestClass.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cerrno>
#include <cstring>

string set_parse_error(const char* text, char* pos, string message)
{
    string out;
    out = string(text).substr(0, pos-text) + "<<:" + message;
    return out;
}

defvaltype convert_text(const char* text,string & error)
{
    char* p = (char*)text;
    defvaltype v;
    vector<int> stack;
    vector<defvaltype*> tree;
    tree.push_back(&v);
    enum {
        hash,
        array,
        str,
        invalue,
        invalue2
    };
    error = "";

    vector<string> key;
  
    while (*p != 0)
    {
        if (*p == '$' || *p == '{' || *p == '[')
        {
            if (*p == '$')
                p++;
            if (*p != 0 && *p == '{')
            {
                if (stack.size()==0)
                    tree.back()->level = make_shared < unordered_map<string, defvaltype> >();
                else
                {
                    if (stack.back() == array)
                    {
                        defvaltype v2;
                        v2.level = make_shared<unordered_map<string, defvaltype> >();
                        tree.back()->array->push_back(v2);
                        tree.push_back(&tree.back()->array->back());
                    }
                    else  if (stack.back() == invalue || stack.back() == invalue2)
                    {
                        defvaltype v2;
                        v2.level = make_shared<unordered_map<string, defvaltype> >();
                        (*tree.back()->level)[key.back()] = v2;
                        tree.push_back(&(*tree.back()->level)[key.back()]);
                        key.pop_back();
                       

                    }
                    else
                    {
                        error = set_parse_error(text, p, "${ is in bad context");
                        return v; // error

                    }

                }
                stack.push_back(hash);
            }
            else if (*p != 0 && *p == '[')
            {
              
               
                if (stack.size() == 0)
                    tree.back()->array = make_shared < vector<defvaltype> >();
                else
                {
                    if (stack.back() == array)
                    {
                        defvaltype v2;
                        v2.array = make_shared<vector<defvaltype> >();
                        tree.back()->array->push_back(v2);
                        tree.push_back(&tree.back()->array->back());
                    }
                    else  if (stack.back() == invalue ||  stack.back() == invalue2)
                    {
                        defvaltype v2;
                        v2.array = make_shared<vector<defvaltype> >();
                        (*tree.back()->level)[key.back()] = v2;
                        tree.push_back(&(*tree.back()->level)[key.back()]);
                        key.pop_back();
                        

                    }
                    else
                    {
                        error = set_parse_error(text, p, "$[ is in bad context");
                        return v; // error

                    }

                }
                stack.push_back(array);
            }
            else
            {
                error = set_parse_error(text, p, "unexpected char after $, expecting { or [");
                return v; // error

            }
        }
        else if (*p == ':')
        {
            p++;
            char* p0 = p; // reading the key
            while (*p != 0 && *p!='(')
            {
                p++;
            }
            if (*p == 0)
            {
                error = set_parse_error(text, p, "text is ended too early");
                return v; // error
            }

            char* p1 = p;
            
            // we have key value at this point
            key.push_back ( string(p0, p1));
            if (stack.empty())
            {
                error = set_parse_error(text, p, ": without context, expecting it inside hash");
                return v; // error
            }
            if (stack.back()!=hash)
            {
                error = set_parse_error(text, p, ": in bad context, expecting it inside hash");
                return v; // error
            }
            stack.push_back(invalue);
            

        }
        else if (*p == '\"') // reading a string
        {
            if (stack.empty())
            {
                error = set_parse_error(text, p, "\" without context, expecting it in some context");
                return v; // error
            }
            string value;
            p++;
            char* p0 = p;
            while (*p != 0 && *p != '"')
            {
                if (*p == '\\')
                {
                    p++;
                    if (*p != 0)
                    {
                        if (*p == 't')
                            value += "\x09";
                        else
                        if (*p == 'n')
                            value += "\x0a";
                        else if (*p == 'r')
                            value += "\x0d";
                        else if (*p == 'v')
                            value += "\x0b";
                        else if (*p == '\\')
                            value += "\\";
                        else
                            value += *p;
                        p++;
                    }
                 
                }
                else
                {
                    value += *p;
                    p++;
                }

            }
            if (*p != 0 && *p != '"' || *p == 0)
            {
                error = set_parse_error(text, p, "missing ending \"");
                return v; // error
            }

            if (stack.back() == invalue || stack.back() == invalue2)
            {
                defvaltype v;
                v.val = value;
                (*tree.back()->level)[key.back()] = v;
                key.pop_back();
                p++;
                if (stack.back() == invalue && *p != 0 && *p != ')' || *p == 0)
                {
                    error = set_parse_error(text, p, "missing )");
                    return v; // error
                }
                stack.pop_back();
            }
            else if (stack.back() == hash)
            {
                key.push_back(value);

            }
           

        }
        else if (*p == '=')
        {
            p++;
            if (*p == 0)
            {
                error = set_parse_error(text, p, "text is ended too early, expecting >, that is =>");
                return v; // error
            }
            if (*p != '>')
            {
                error = set_parse_error(text, p, "unexpected char, expecting >, that is =>");
                return v; // error
            }
            stack.push_back(invalue2);

        }
        else if (*p == ')')
        {
            if (stack.empty())
            {
                error = set_parse_error(text, p, ") without context, expecting it inside hash");
                return v; // error
            }
            else if (stack.back() == invalue)
            {
                stack.pop_back();
            }
            else
            {
                error = set_parse_error(text, p, ") in bad context");
                return v; // error
            }

        }
        else if (*p == '}')
        {
            if (stack.empty())
            {
                error = set_parse_error(text, p, "} without context, expecting it inside hash");
                return v; // error
            }
            if (stack.back() == invalue2)
            {
                stack.pop_back();
            }
            else if (stack.back() != hash)
            {
                error = set_parse_error(text, p, "} in bad context, expecting it inside hash");
                return v; // error
            }
            stack.pop_back();
            tree.pop_back();
        }
        else if (*p == ']')
        {
            if (stack.empty())
            {
                error = set_parse_error(text, p, "] without context, expecting it inside list");
                return v; // error
            }
            if (stack.back() != array)
            {
                error = set_parse_error(text, p, "] in bad context, expecting it inside list");
                return v; // error
            }
            stack.pop_back();
            tree.pop_back();
        }
        else if (*p == ',' ) // ignore
        {
            if (stack.back() == invalue2)
            {
                stack.pop_back();
            }
        }
        else if (*p == ' ')
        {

        }
        else // probably a number 
        {
            char* p0 = p;
            p++;
            while (*p != 0 && *p != ')' && *p != ',' && *p != '}' && *p != ']')
            {
                p++;
            }
            if (*p == 0)
            {
                error = set_parse_error(text, p, "text is ended too early while probably reading a number, expecting something");
                return v; // error
            }
            if (stack.empty())
            {
                error = set_parse_error(text, p, "probably reading number but no context, expecting some context, hash or array");
                return v; // error
            }
            if (stack.back() == invalue || stack.back() == invalue2)
            {
                defvaltype v;
                v.val = string(p0,p);
                (*tree.back()->level)[key.back()] = v;
                key.pop_back();
               
                if (stack.back() == invalue && *p != 0 && *p != ')' || *p == 0)
                {
                    error = set_parse_error(text, p, "missing )");
                    return v; // error
                }
                if (stack.back() != invalue)
                {
                    p--;
                }
                stack.pop_back();
            }
            else if (stack.back() == array)
            {
                defvaltype v;
                v.val = string(p0, p);
                (*tree.back()->array).push_back(v);
                p--;
            }

        }
        if (*p != 0)
        {
            p++;
        }

    }

    return v;
}

/*method render ( $comp ){

        my $html;
        if $comp ~~ Array {
            $html = self!render_array( $comp );
        } elsif $comp ~~ Hash {
            $html = self!render_hash( $comp );
        } else {
    		$html = $comp;
        }

        return $html;
    }*/

string TemplateNestClass::render(const defvaltype& comp)
{
    //self.comment_delims = @comment_delims_defaults unless grep{ $_ }, @!comment_delims;
   //self.token_delims = @token_delims_defaults unless grep{ $_ }, @!token_delims;
    if (comment_delims->size() == 0)
        std::copy(&comment_delims_defaults[0], &comment_delims_defaults[2], comment_delims);
    // comment_delims = comment_delims_defaults;
    if (token_delims->size() == 0)
        //token_delims = token_delims_defaults;
        std::copy(&token_delims_defaults[0], &token_delims_defaults[2], token_delims);
    die = "";
    string html;
    if (comp.array!=nullptr && comp.array->size() != 0)
        html = render_array(*comp.array);
    else if (comp.level != nullptr && comp.level->size() != 0)
        html = render_hash(*comp.level);
    else
        html = comp.val;
    return html;
}
/*method !render_hash( %h ){
        #say "render hash: " ~ %h<NAME>;
        my $template_name = %h{ self.name_label };

        die 'Encountered hash with no name_label ("' ~ self.name_label ~ '"): ' ~ Dump( %h ) unless $template_name;

        my %param;

        for %h.keys -> $k {
            next if $k eq self.name_label;
            %param{$k} = self.render( %h{$k} );
        }

        my $template = self!get_template( $template_name );
        my $html = self!fill_in( $template_name, $template, %param );

        if self.show_labels {

            my $ca = self.comment_delims[0];
            my $cb = self.comment_delims[1];

    		$html = "$ca BEGIN $template_name $cb\n$html\n$ca END $template_name $cb\n";
            
        }

        return $html;

    }*/


string TemplateNestClass::render_hash(unordered_map<string, defvaltype> h) {
    //#say "render hash: " ~ %h<NAME>;
    string template_name = h[name_label].val;

    if (template_name.empty())
    {
        die = "Encountered hash with no name_label(\"" + name_label + "\")";
        return "";
    }

    unordered_map<string, string> param;
    for (auto& k : h)
    {
        if (k.first == name_label)
            continue;
        param[k.first] = render( k.second);
    }

    string template1 = get_template( template_name );
    string html = fill_in( template_name, template1, param );

    if (show_labels) {

        const string & ca = comment_delims[0];
        const string & cb = comment_delims[1];

        html = ca+" BEGIN "+template_name+" "+cb+"\n"+ html + "\n" + ca + " END " + template_name + " " + cb + "\n";
    }

    return html;

}

/*method !render_array( @arr ){
        #say "render array";
        my $html = '';
        for @arr -> $comp {
            $html ~= self.render( $comp );
        }
        return $html;

    }*/

string TemplateNestClass::render_array(vector<defvaltype> arr) {
    //#say "render array";
    string html = "";
    for (auto& v : arr) {
        html += render(v);
    }
    return html;
}




/*method !get_template(Str $template_name) {
    #say "get template";

    my $template = '';
    if self.template_hash{
        $template = self.template_hash{$template_name};
    }
    else {

        my $filename = self.template_dir.IO.add(
            $template_name ~self.template_ext
        );

        $template = slurp $filename;


    }

    $template ~~s / \n$//;
        return $template;
}*/

void remove_trailing_whitespace(string& s)
{
    if (s.size() == 0)
        return;
    size_t p = s.size()-1;
    while (p >=0)
    {
        if (s[p] == 32 || s[p] == 10 || s[p] == 13 || s[p] == 9)
        {
            p--;
        }
        else
            break;
    }
    s = s.substr(0, p + 1);


}



string TemplateNestClass::get_template(string template_name) {
    

    string template1 = "";
    if (template_hash.size()!=0) {
        template1 = template_hash.at(template_name);
    }
    else {
        auto p = std::filesystem::path(template_dir) / (template_name+ template_ext);
       
        string filename = p.string();
        // + template_ext

        //template = slurp $filename;
        std::ifstream t(filename);
        if (t.fail())
        {
            char buf[1000];
            strerror_s(buf, 1000, errno);
            die = "could not open " + filename + " reason:" + buf;
            return "";
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        template1 = buffer.str();

    }

    //$template ~~s / \n$//;
    remove_trailing_whitespace(template1);
    return template1;
}



/*method params($template_name) {
    #say "params";

    my $esc = self.escape_char;
    my $template = self!get_template($template_name);
    my @frags = $template.split(/ $esc$esc / );
    my $tda = self.token_delims[0];
    my $tdb = self.token_delims[1];

    my% rem;
    for @frags.keys->$i{
        my @f = @frags[$i] ~~m:g / <!after $esc> $tda <(.* ? )> $tdb / ;
        for @f->$f {
            my Str $elem = $f.Str;
            $elem ~~s / ^ \s* //;
            $elem ~~s / \s * $//;
            % rem{$elem} = True;
        }
    }

    my @params = % rem.keys.sort;
    return @params;
}*/

vector<string> split(const string& s, const string& del)
{
    vector<string> res;
    auto p = s.find(del);
    size_t p0 = 0;
    if (p == string::npos)
    {
        res.push_back(s);
        return res;
    }
    while (p != string::npos)
    {
        res.push_back(s.substr(p0, p-p0));
        p0 = p+ del.size();
        p = s.find(del, p0);
    }
    if (p0 < s.size())
    {
        res.push_back(s.substr(p0));
    }
    return res;
  
}


vector<string> sort(const vector<string>& v)
{
    vector<string> res = v;
    std::sort(res.begin(), res.end());
    return res;
}

vector<string> TemplateNestClass::params(string template_name) {
    
   


    string esc = escape_char;
    string template1 = get_template(template_name);
    vector<string> frags = split(template1, esc + esc); // $template.split(/ $esc$esc / );
   
    vector<string> rem;
    for (auto f : frags)
    { 
        vector<string> res = params_in(f);
        for (auto s : res)
        {
            rem.push_back(s);
        }
    }
    
    return sort(rem);
}

/* method !fill_in( Str $template_name, Str $template, %params){

        my Str $esc = self.escape_char;
        my Str @frags;

        if $esc {
            @frags = $template.split( /$esc$esc/ );
        } else {
            @frags = ( $template );
        }

        for %params.keys -> $param_name {
            #say "for params.keys";

            my $param_val = %params{$param_name};

            my Bool $replaced = False;

            if self.fixed_indent { #if fixed_indent we need to add spaces during the replacement
                for @frags.keys -> $i {
                    #say "frags.keys";
                    my Regex $rx = self!token_regex( $param_name );
                    my Match @spaces_repl = @frags[$i] ~~ m:g/(<-[\S\r\n]>*) <$rx>/;

                    while @spaces_repl {
                        #say "spaces_repl: " ~ Dump( @spaces_repl );
                        #say "while";
                        my Match $repl = shift @spaces_repl;
                        my Match $sp = $repl.list[0];

                        my Str $param_out = $param_val;
                        #say "param out before: " ~ $param_out;
                        $param_out ~~ s:g/\n/\n$sp/;
                        $param_out = $sp ~ $param_out;
                        #say "param out after: " ~ $param_out;

                        if $esc {
                            $replaced = True if @frags[$i] ~~ s/<!after $esc> $repl/$param_out/;
                        } else {
                            $replaced = True if @frags[$i] ~~ s/$repl/$param_out/;
                        }
                    }
                }
            } else {
                for @frags.keys -> $i {
                    #say "for ffk";
                    my Regex $rx = self!token_regex( $param_name );
                    #say "regex: " ~ $rx.gist;
                    #say "frag: " ~ @frags[$i];
                    #say "param_val: " ~ $param_val.gist;
                    #say "param_name: " ~ $param_name.Str;
                    #say "m: " ~ $m.gist;
                    $replaced = True if @frags[$i] ~~ s:g/<$rx>/$param_val/;
                    #say "end of ffk";
                }
            }

            if self.die_on_bad_params and not $replaced  {
                die "Could not replace template param '$param_name': token does not exist in template '$template_name'";
            }
        }

        #say "finished for";

        for @frags.keys -> $i {
            #say "for frags keys";

            if self.defaults {
                my Str @rem = self!params_in( @frags[$i] );
                #say "defaults rem: " ~ Dump( @rem );
                my $char = self.defaults_namespace_char;
                #say "namespace char: $char";
                for @rem -> $name {
                    my Str @parts = ( $name );
                    @parts = $name.split($char) if $char;

                    my $val = self!get_default_val( self.defaults, @parts );
                    my $rx = self!token_regex( $name );
                    @frags[$i] ~~ s:g/<$rx>/$val/;
                }
            }

            my Regex $rx = self!token_regex;
            #say "frags regex: " ~ $rx.raku;
            #say "frag: " ~ @frags[$i];
            @frags[$i] ~~ s:g/<$rx>//;
            #say "after frags regex";
        }

        if $esc {
            for @frags.keys -> $i {
                @frags[$i] ~~ s:g/$esc//;
            }
        }

        my $text = $esc ?? @frags.join( $esc ) !! @frags[0];
        return $text;
    }
*/

void skip_space(string& s, size_t & p)
{
  
    while (p < s.size())
    {
        if (s[p] == 32  || s[p] == 9)
        {
            p++;
        }
        else
            break;
    }
}
// points to the first space or non space if there is no space
void skip_space_backwards(const string& s, size_t& p)
{
    p--;
    while (p >=0)
    {
        if (s[p] == 32 || s[p] == 9)
        {
            p--;
        }
        else
            break;
    }
    p++;
}

void set_indent_all(string& s, const string& indent)
{
    size_t p = 0;
    string o;
    bool e = false;
    size_t p0 = p;
    while (p < s.size())
    {
        
        if (s[p] == 13)
        {
            p++;
            e = true;
            if (p < s.size() && s[p] == 10)
            {
                p++;
            }
        }
        else if (s[p] == 10)
        {
            p++;
            e = true;
        }
        else
            p++;

        if (e)
        {
            o += s.substr(p0, p - p0) + indent;
            e = false;
            p0 = p;
        }
    }
    if (p0 < s.size())
    {
        o += s.substr(p0);
    }

    s = o;

}

string & TemplateNestClass::died()
{
    string o = die;
   
    return die;
}

string replace_all(const string& s, const string& r, const string & replacement)
{
    string res;
    auto p = s.find(r);
    size_t p0 = 0;
    if (p == string::npos)
    {
        return s;
    }
    while (p != string::npos)
    {
        res+= s.substr(p0, p-p0) + replacement;
        p0 = p + r.size();
        p = s.find(r, p0);
    }
    if (p0 < s.size())
    {
        res += s.substr(p0);

    }
    return res;

}

string join(const vector<string>& s, const string& del)
{
    std::stringstream ss;
    bool first = true;
    for (auto & s1:s)
    {
        if (first)
        {
            ss << s1;
            first = false;
        }
        else
            ss << del << s1;

    }
    return ss.str();

}


string TemplateNestClass::fill_in(const string & template_name, const string & template1, const unordered_map<string,string> & params) {

    string esc = escape_char;
    vector<string> frags;

    if (!esc.empty()){
        frags = split(template1, esc+esc );
    }
    else {
        frags.push_back(template1);
       
    }

    for (auto& kv : params)
    {
        const string& param_name = kv.first;


        const string& param_val = kv.second;

        bool replaced = false;

        if (fixed_indent) { //if fixed_indent we need to add spaces during the replacement
            for (auto& f : frags) {
                //for @frags.keys->$i {

                size_t p = 0;

                // my Regex $rx = self!token_regex($param_name);
                 //my Match @spaces_repl = @frags[$i] ~~m:g / (<-[\S\r\n]>*) < $rx > / ;
                while (f.size() > p)
                {
                    //size_t p0 = p;
                    //skip_space(f, p);
                    //size_t p1 = p;
                    //string sp = f.substr(p0, p1 - p0);
                    bool found;
                    size_t p0;
                    if (!token_regex(param_name, f, p0,p, false,found))
                        break;
                    if (found)
                    {
                        size_t ps = p0;
                        skip_space_backwards(f,ps);
                        string sp = f.substr(ps, p0 - ps);
                        //say "spaces_repl: " ~Dump(@spaces_repl);
                        //say "while";
                        //my Match $repl = shift @spaces_repl;
                        //my Match $sp = $repl.list[0];

                        string param_out = param_val;
                        //say "param out before: " ~$param_out;
                        //$param_out ~~s:g / \n / \n$sp / ;
                        set_indent_all(param_out, sp);
                        param_out = sp + param_out;
                        //say "param out after: " ~$param_out;

                        if (esc.size() != 0)
                        {
                            if (ps >= esc.size() && f.substr(ps - esc.size(), esc.size()) == esc)
                            {

                            }
                            else
                            {
                                replaced = true;
                                f.replace(ps, p - ps, param_out);
                                p = ps + param_out.size();
                            }
                        }
                        else
                        {
                            replaced = true;
                            f.replace(ps, p - ps, param_out);
                            p = ps + param_out.size();

                        }

                        /*  if (!esc.empty()) {

                              $replaced = True if @frags[$i] ~~s / <!after $esc> $repl / $param_out / ;
                          }*/

                    }
                }

            }
        }
        else {
            for (auto& f : frags) {
                //say "for ffk";
                //my Regex $rx = self!token_regex($param_name);
                //say "regex: " ~$rx.gist;
                //say "frag: " ~@frags[$i];
                //say "param_val: " ~$param_val.gist;
                //say "param_name: " ~$param_name.Str;
                //say "m: " ~$m.gist;
                size_t p = 0;
                while (f.size() > p)
                {
                    bool found;
                    size_t p0;
                    if (!token_regex(param_name, f, p0,p, false,found))
                        break;
                    if (found)
                    {
                        replaced = true;
                        f.replace(p0, p - p0, param_val);
                        p = p0 + param_val.size();
                    }
                }
                //replaced = True if @frags[$i] ~~s:g / <$rx> / $param_val / ;
                //say "end of ffk";
            }
        }
        if (die_on_bad_params && ! replaced) {
               die = "Could not replace template param '"+param_name+"': token does not exist in template '"+template_name+"'";
               return "";
        }

    }
        

    //say "finished for";

    for (auto& f : frags) {
        //say "for frags keys";

        if (defaults.size()!=0) {
            vector<string> rem = params_in(f);
            //say "defaults rem: " ~Dump(@rem);
            const string & char1 = defaults_namespace_char;
            //say "namespace char: $char";
            for (auto name:rem)
            {
                vector<string> parts = { name };
                if (!char1.empty())
                    parts = split(name, char1);

                string val = get_default_val(defaults, parts);
                size_t p = 0;
                while (f.size() > p)
                {
                    bool found;
                    size_t p0;
                    if (!token_regex(name, f, p0,p, false,found))
                        break;
                    if (found)
                    {
                        f.replace(p0, p - p0, val);
                        p = p0 + val.size();
                    }
                }

               // my $rx = self!token_regex($name);
               // @frags[$i] ~~s:g / <$rx> / $val / ;
            }
        }

        //my Regex $rx = self!token_regex;
        //#say "frags regex: " ~$rx.raku;
        //#say "frag: " ~@frags[$i];
        size_t p = 0;
        while (f.size() > p)
        {
            bool found;
            size_t p0;
            if (!token_regex("", f, p0,p, false,found))
                break;
            if (found)
            {
                f.replace(p0, p - p0, "");
                p = p0;
            }
        }
        //@frags[$i] ~~s:g / <$rx>//;
        //#say "after frags regex";
    }

        if (!esc.empty()){
            for (auto& f : frags) {
                
                f = replace_all(f, esc, "");
                //@frags[$i] ~~s:g / $esc//;
            }
        }

    string text = !esc.empty() ? join(frags,esc) :frags[0];
    return text;
}


/*method !params_in(Str $text) {

    my Str $esc = self.escape_char;
    my Str $tda = self.token_delims[0];
    my Str $tdb = self.token_delims[1];

    #my @rem;
    my Match @m;
    if $esc{
        @m = $text ~~m:g / <!after $esc> $tda \s + <(.* ? )> \s + $tdb / ;
        #@rem = grep { $_.chunks[0].keys[0] }, @m;
    }
    else {
        @m = $text ~~m:g / $tda \s + <(.* ? )> \s + $tdb / ;
        #@rem = grep{ $_.chunks[0].keys[0] }, @m;
    }
    #say "m: " ~Dump(@m);
    #say "arr rem: " ~Dump(@rem);

    my Bool% rem;
    for @m->$name{
        % rem{ $name } = True
    }
    #say "rem: " ~Dump(% rem);

    return % rem.keys;
}*/

bool skip_whitespace(const string& s, size_t& p)
{
    size_t pc = p;
    while (s.size() > p)
    {
        if (s[p] == 32 || s[p] == 10 || s[p] == 13 || s[p] == 9)
        {
            p++;
        }
        else
            break;
    }
    return pc != p;

}

bool find_next_whitespace(const string& s, size_t & p)
{
    size_t pc = p;
  
    while (s.size() > p)
    {
        if (s[p] == 32 || s[p] == 10 || s[p] == 13 || s[p] == 9)
        {
           
            return true;
        }
        else
            p++;
    }
    p = pc;
    return false;

}
vector<string> TemplateNestClass::params_in(string text) {

    string esc = escape_char;
    string tda = token_delims[0];
    string tdb = token_delims[1];
    vector<string> keys;
    
   
        size_t p = 0;
        while (text.size() > p)
        {
            auto p2 = text.find(tda, p);
            if (p2 == string::npos)
                return keys;
            if (esc.size()!=0)
                if (p2 >= esc.size() && text.substr(p2 - esc.size(), esc.size()) == esc)
                {
                    p = p2;
                    p += tda.size();
                    continue;
                }
            p = p2;
            p += tda.size();
            if(!skip_whitespace(text, p))
                continue;
            p2 = p;
            if (!find_next_whitespace(text, p2))
                continue;
            size_t p3 = p2;
            if (!skip_whitespace(text, p3))
            {
                continue;
            }
            if (text.substr(p3, tdb.size()) != tdb)
                continue;

            keys.push_back(text.substr(p,p2-p));
            p = p3 + tdb.size();
        }

   
        return keys;
}



/*method !get_default_val(% def, Str @parts) {
    

    if @parts == 1 {
        my $val = % def{ @parts[0] } || '';
        return $val;
    }
    else {
        my $ref_name = shift @parts;
       
        my $new_def = % def{ $ref_name };
       
        my% new_def = % def{ $ref_name };
        
        return '' unless % new_def;
        return self!get_default_val(% new_def, @parts);
    }
}*/

string shift(vector<string>& v) {
    if (!v.empty())
    {
        string f = v.front();
        v.erase(v.begin());
        return f;
    }
    else
    {
        return "";
    }
}


string TemplateNestClass::get_default_val(const unordered_map<string, defvaltype> & def, vector<string> parts)
{
    if (parts.size() == 1)
    {
        auto f = def.find(parts[0]);
        string val = f != def.end() ? f->second.val : "";
        return val;
    }
    else {
        string ref_name = shift (parts);

        auto new_def = def.find(ref_name);
        //my% new_def = % def{ $ref_name };
        if (new_def==def.end() || new_def->second.level==nullptr || new_def->second.level->size() == 0)
            return "";

        //return '' unless % new_def;
        return get_default_val(*(new_def->second.level), parts);
    }


}

/*method !token_regex(Str $param_name ? ) {
    #say "token regex begins";
    my Str $esc = self.escape_char;
    my Str $tda = self.token_delims[0];
    my Str $tdb = self.token_delims[1];

    my $param_title = $param_name || '.*?';

    #say "tda: $tda";
    #say "tdb: $tdb";
    #say "param_title: $param_title";

    my Regex $token_regex = / $tda \s + <$param_title> \s + $tdb / ;
    if $esc{
        $token_regex = / <!after $esc> $tda \s + <$param_title> \s + $tdb / ;
    }
    #say "token regex ends";
    return $token_regex;
}*/


// returns true if searching can be continued
bool TemplateNestClass::token_regex(const string& param_name, const string& text, size_t & p0,size_t & p, bool fixed_pos,bool & found)
{
    const string & esc = escape_char;
    const string &tda = token_delims[0];
    const string & tdb = token_delims[1];

    string param_title = param_name;
    found = false;


    if (text.size() > p)
    {
        size_t p2;
        if (fixed_pos)
        {
            if (text.substr(p, tda.size()) != tda)
                return false;
            p2 = p;
        }
        else
           p2 = text.find(tda, p);
       
        if (p2 == string::npos)
            return false;
        p0 = p2;
        if (esc.size() != 0)
            if (p2 >= esc.size() && text.substr(p2 - esc.size(), esc.size()) == esc)
            {
                p = p2;
                p += tda.size();
                return true;
            }
        p = p2;
        p += tda.size();
        if (!skip_whitespace(text, p))
            return true;
        p2 = p;
        if (!find_next_whitespace(text, p2))
            return true;
        size_t p3 = p2;
        if (!skip_whitespace(text, p3))
        {
            return true;
        }
        if (text.substr(p3, tdb.size()) != tdb)
            return true;

        if (text.substr(p, p2 - p) != param_name && !param_name.empty())
            return true;
        p = p3 + tdb.size();
        found = true;
        return true;
    }
    return false;
}


