#include <string.h>
#include <stdio.h>
#include <iostream>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;
using namespace std;

int main(void)
{
    cout <<"hello"<<endl;
    xml_document<char> doc;
    try{
        file<char> xmlFile("C:\\Users\\zacha\\Desktop\\dblp.xml");
        doc.parse<0>(xmlFile.data());
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }
    
    
	cout << "Name of my first node is: " << doc.first_node()->name() << "\n";
    xml_node<> *node = doc.first_node(doc.first_node()->name());
    cout << "Node MyBeerJournal has value " << node->value() << "\n";
    for (xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
    {
        cout << "Node MyBeerJournal has attribute " << attr->name() << " ";
        cout << "with value " << attr->value() << "\n";
    }
}