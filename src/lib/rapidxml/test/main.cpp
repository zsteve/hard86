#include <iostream>
#include <cstdio>
#include "../rapidxml.hpp"
#include "../rapidxml_print.hpp"

using namespace std;
using namespace rapidxml;

int main(){
	FILE* f=fopen("test.xml", "r");
	int fsize;
	{
		int pos=ftell(f);
		fseek(f, 0, SEEK_END);
		fsize=ftell(f);
		fseek(f, pos, SEEK_SET);
	}
	char* txt=new char[fsize];
	fread(txt, 1, fsize, f);
	fclose(f);
	cout << txt  << endl;
	xml_document<> doc;
	try{
		doc.parse<0>(txt);
		xml_node<> *node=doc.first_node("Node1");
		cout << node->name() <<  endl;
		cout << "Node value : " << node->value() << endl;
		cout << "Node attrib: " << node->first_attribute()->name() << endl;
		node=node->first_node("Node2");
		cout << node->name() <<  endl;
		cout << "Node value : " << node->value() << endl;
		cout << "Node attrib: " << node->first_attribute()->name() << endl;
	}catch(rapidxml::parse_error e){
		cout << "Exception : " << e.what() << endl;
	}

	cout << doc << endl;
	return 0;
}
