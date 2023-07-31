#include <iostream>
#include"SkipList.h"
using namespace std;
#include<string>
int main() {
    SkipList<int, std::string> skipList(7);
    skipList.inserted_node(1,"HAN");
    skipList.inserted_node(2,"劲");
    skipList.inserted_node(3,"FENG");
    skipList.inserted_node(5,"芜湖");
    skipList.inserted_node(8,"778");
    skipList.inserted_node(19,"857");
    skipList.inserted_node(13,"886");
    skipList.inserted_node(15,"110");
    skipList.inserted_node(18,"蚌埠");
    skipList.inserted_node(23,"kk7");

    skipList.display_skipList();

    skipList.search_node(23);



}
