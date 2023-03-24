// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TemplateNestClass.h"

defvaltype convert_text(const char* text, string& error);

defvaltype convert_jsontext(const char* text, string& error);

/*my $page = {
		NAME => 'page',
		contents => [{
			NAME => 'box',
			title => 'First nested box'
		}]
	};

	push @{$page->{contents}},{
		NAME => 'box',
		title => 'Second nested box'
	};

	my $nest = Template::Nest->new(
		template_dir => '/html/templates/dir',
        fixed_indent => 1
	);
    print $nest->render( $page );
    */

void rendertest1()
{
    string data = "${:NAME(\"page\"), :contents($[{:NAME(\"box\"), :title(\"First nested box\")}, {:NAME(\"box\"), :title(\"Second nested box\")}])}";
    auto o = new TemplateNestClass();
    string error;
    o->template_dir = R"(D:\m\cpp\TemplateNest\tests\)";
    o->fixed_indent = true;
    o->name_label = "NAME";
    defvaltype v = convert_text(data.c_str(), error);
    cout << o->render(v);


}
string replace_all(const string& s, const string& r, const string& replacement);
void rendertest2()
{
    auto o = new TemplateNestClass();
    string error;
    o->template_dir = R"(D:\m\cpp\TemplateNest\ConsoleApplication1\templates\)";
    o->fixed_indent = false;
    o->name_label = "TEMPLATE";
    o->token_delims[0] = "<!--%";
    o->token_delims[1] = "%-->";
    string data = "${:TEMPLATE(\"t1\")}";
    defvaltype v = convert_text(data.c_str(), error);
    string rendered_script = o->render(v);
    // second level
    rendered_script=replace_all(rendered_script, "\"", "\\\"");
    rendered_script=replace_all(rendered_script, "\x0a", "\\n");

    data = "${:TEMPLATE(\"t2\"),:contents(\""+ rendered_script +"\")}";
    v = convert_text(data.c_str(), error);
    string rendered_section = o->render(v);
    int b = 4;
}

void pythonrender()
{
    auto o = new TemplateNestClass();
    string error;
    o->template_dir = R"(D:\m\python\templatenest\template-nestdll-1.0.0\template_nestdll\tests\templates)";
    o->fixed_indent = false;
    o->name_label = "NAME";
    o->token_delims[0] = "<%";
    o->token_delims[1] = "%>";
    string data = "{\"NAME\": \"page\", \"contents\": [{\"NAME\": \"box\", \"title\": \"First nested box\"}, {\"NAME\": \"box\", \"title\": \"Second nested box\"}]}";
    defvaltype v = convert_jsontext(data.c_str(), error);
    string rendered_script = o->render(v);

    std::cout << rendered_script << "\n";
}

void pythontestbadparams()
{
    

    auto o = new TemplateNestClass();
    string error;
    o->template_dir = R"(D:\m\python\templatenest\template-nestdll-1.0.0\template_nestdll\tests\templates)";
    o->fixed_indent = false;
    o->name_label = "NAME";
    o->token_delims[0] = "<%";
    o->token_delims[1] = "%>";
    string data = R"({ "NAME": "table", "rows" : [{"NAME": "tr", "cols" : {"NAME": "tr", "bad_param" : "stuff"}}, { "NAME": "tr", "cols" : {"NAME": "td"} }] })";
    o->die_on_bad_params = true;
    defvaltype v = convert_jsontext(data.c_str(), error);
    string rendered_script = o->rendertop(v);

    std::cout << o->die << "\n";

}

void test()
{
    /* my %table =
        NAME => 'table',
        rows => [{
            NAME => 'table_row',
            name => 'Sam',
            job => 'programmer'
        }, {
            NAME => 'table_row',
            name => 'Steve',
            job => 'soda jerk'
        }];*/
    string error;
    //defvaltype v = convert_text("${\":\" => 25, :Ben(23), :Lia(${:b(4)})}", error);

    //defvaltype v = convert_text("${ \"2\" => $[1, 2, 3] }", error);

    

    defvaltype v = convert_text("${ \"2\" => $[1, 2, {:o(5)}] }", error);
    
}

void jsontest()
{
    string error;
   


    defvaltype v = convert_jsontext("{\"NAME\": \"page\", \"contents\": [{\"NAME\": \"box\", \"title\": \"First nested box\"}, {\"NAME\": \"box\", \"title\": \"Second nested box\"}]}", error);
    std::cout << "errpr " << error << "\n";
}


int main()
{
    //test();
   // jsontest();
    //pythonrender();
    rendertest2();
   // pythontestbadparams();
}


