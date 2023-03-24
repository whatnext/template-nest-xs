#define TEMPLATENEST_API __declspec(dllexport)

class TemplateNestClass {
public:
    __int64 i=  5;


};

extern  "C"  TEMPLATENEST_API  void templatenest_init(__int64 * object);

extern  "C"  TEMPLATENEST_API  void templatenest_d(__int64   object);
extern  "C"  TEMPLATENEST_API void templatenest_g(__int64 p, __int64* i);

void templatenest_init(__int64* object)
{
    //TemmplNest
    *object = (__int64) new TemplateNestClass();

}

void templatenest_g(__int64 p, __int64* i)
{
   *i= ((TemplateNestClass*)p)->i;

}

void templatenest_d(__int64 p)
{
    //TemmplNest
    delete (void*)p;

}

