g++ -fPIC -std=c++17 -shared connector.cpp -O3 -I ../ConsoleApplication1/ ../ConsoleApplication1/TemplateNestClass.cpp -lstdc++fs -o libtemplatenest.so
