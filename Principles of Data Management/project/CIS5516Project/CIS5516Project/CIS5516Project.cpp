// CIS5516Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <iostream>
#include <pqxx/pqxx>
#include <string.h>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <ctime>
#include <set>
#include <map>
using namespace rapidxml;
using namespace std;
int main()
{
	string publicationSqlHolder,personSqlHolder, pub2personSqlHolder;
	size_t pubID = 0;
	set<string> person;
	map<string, string> affiliation;
	xml_document<char> doc;
	file<char> xmlFile("C:\\Users\\zacha\\Desktop\\dblp.xml");
	doc.parse<0>(xmlFile.data());
	xml_node<> *root = doc.first_node("dblp");
	vector<string> publicationTypes = { "article", "inproceedings", "proceedings", "book", "incollection", "phdthesis", "mastersthesis" };
	string publicationSqlHd = "insert into publication_demo(pubID, title, booktitle, pages, pubyear, address, journal, volume, pubnumber, pubmonth, url, ee, cdrom, cite, publisher, note, crossref, isbn, series, school, chapter, publnr) values ";
	string personSqlHd = "insert into person_demo(personname, affiliate, homepage) values ";
	string pub2personSqlHd = "insert into pub2person_demo(pubID, pubtype, personname, personrole) values ";
	string personSqlTl = " on conflict do nothing;";
	string publicationSqlTl = personSqlTl;
	string pub2personSqlTl = personSqlTl;
	vector<string> publicationAttributes = { "title", "booktitle", "pages", "year", "address", "journal", "volume", "number", "month", "url", "ee", "cdrom", "cite", "publisher", "note", "crossref", "isbn", "series", "school", "chapter", "publnr" };
	//vector<string>personAttributes = { "author", "note", "url", "cite", "crossref" };
	try
	{
		size_t batchCnt = 100;
		size_t recordCnt;
		string publicationSqlHolder, personSqlHolder, tmp;
		pqxx::connection C("host=localhost user=postgres dbname=postgres password=postgres");
		pqxx::nontransaction W(C);
		W.exec("truncate pub2person, person, publication;");

		//build a person to school map
		for (auto pubTypeIt = publicationTypes.cbegin(); pubTypeIt != publicationTypes.cend(); ++pubTypeIt)
		{
			string pubType = *pubTypeIt;
			for (xml_node<> *publication = root->first_node(&(pubType[0])); publication; publication = publication->next_sibling(&(pubType[0])))
			{
				xml_node<> *author = publication->first_node("author");
				xml_node<> *editor = publication->first_node("editor");
				xml_node<> *school = publication->first_node("school");
				string personNm, schoolNm;
				if ((!school) || !author && !editor)
				{
					continue;
				}
				schoolNm = school->value();
				boost::replace_all(schoolNm, "'", "''");
				for (; author; author = author->next_sibling("author"))
				{
					personNm = author->value();
					boost::replace_all(personNm, "'", "''");
					affiliation[personNm] = schoolNm;
				}
				for (; editor; editor = editor->next_sibling("editor"))
				{
					personNm = editor->value();
					boost::replace_all(personNm, "'", "''");
					affiliation[personNm] = schoolNm;
				}
			}
		}

		//insert person
		recordCnt = 0;
		clock_t personInsertBegin = clock();
		for (xml_node<> *publication = root->first_node("www"); publication; publication = publication->next_sibling("www"))
		{		
			xml_node<> *author = publication->first_node("author");
			xml_node<> *editor = publication->first_node("editor");
			xml_node<> *url = publication->first_node("url");
			string personNm, urlStr, schoolStr;
			if (!author && !editor)
			{
				continue;
			}
			for (; author; author = author->next_sibling("author"))
			{
				personNm = author->value();
				boost::replace_all(personNm, "'", "''");
				auto it = affiliation.find(personNm);
				if (it != affiliation.end())
				{
					schoolStr = it->second;
					boost::replace_all(schoolStr, "'", "''");
					schoolStr = "'" + schoolStr + "'";
				}
				else
				{
					schoolStr = "NULL";
				}
				personNm = "'" + personNm + "'";
				if (url)
				{
					urlStr = url->value();
					boost::replace_all(urlStr, "'", "''");			
					urlStr = "'" + urlStr + "'";
				}
				else
				{
					urlStr = "NULL";
				}							
				personSqlHolder += ("(" + personNm + "," + schoolStr + "," + urlStr + "),");
				++recordCnt;
			}
			for (; editor; editor = editor->next_sibling("editor"))
			{
				personNm = editor->value();
				boost::replace_all(personNm, "'", "''");
				auto it = affiliation.find(personNm);
				if (it != affiliation.end())
				{
					schoolStr = it->second;
					boost::replace_all(schoolStr, "'", "''");
					schoolStr = "'" + schoolStr + "'";
				}
				else
				{
					schoolStr = "NULL";
				}
				personNm = "'" + personNm + "'";
				if (url)
				{
					urlStr = url->value();
					boost::replace_all(urlStr, "'", "''");
					urlStr = "'" + urlStr + "'";
				}
				else
				{
					urlStr = "NULL";
				}			
				personSqlHolder += ("(" + personNm + "," + schoolStr + "," + urlStr + "),");
				++recordCnt;
			}
			if (recordCnt >= batchCnt)
			{
				personSqlHolder.pop_back();
				W.exec(personSqlHd + personSqlHolder + personSqlTl);
				personSqlHolder.clear();
				recordCnt = 0;
				break;/*insert a batch of records for demo purpose*/
			}
		}
		if (recordCnt > 0)
		{
			personSqlHolder.pop_back();
			W.exec(personSqlHd + personSqlHolder + personSqlTl);
			personSqlHolder.clear();
			recordCnt = 0;
		}
		clock_t personInsertEnd = clock();
		double personInsertTime = double(personInsertEnd - personInsertBegin) / CLOCKS_PER_SEC;
		

		//update person table with affility
		/*for (auto pubTypeIt = publicationTypes.cbegin(); pubTypeIt != publicationTypes.cend(); ++pubTypeIt)
		{
			string pubType = *pubTypeIt;
			for (xml_node<> *publication = root->first_node(&(pubType[0])); publication; publication = publication->next_sibling(&(pubType[0])))
			{
				xml_node<> *author = publication->first_node("author");
				xml_node<> *editor = publication->first_node("editor");
				xml_node<> *school = publication->first_node("school");
				string personNm, urlStr;
				if (!author && !editor)
				{
					continue;
				}
				for (; author; author = author->next_sibling("author"))
				{
					personNm = author->value();
					if (school)
					{
						affiliation[personNm] = school->value();
					}
					if (person.find(personNm) == person.end())
					{
						person.insert(personNm);
						boost::replace_all(personNm, "'", "''");
						W.exec("insert into person(person_name) values ('" + personNm + "') on conflict do nothing;");
					}
				}
				for (; editor; editor = editor->next_sibling("editor"))
				{
					personNm = editor->value();
					if (school)
					{
						affiliation[personNm] = school->value();
					}
					if (person.find(personNm) == person.end())
					{
						person.insert(personNm);
						boost::replace_all(personNm, "'", "''");
						W.exec("insert into person(person_name) values ('" + personNm + "') on conflict do nothing;");
					}
				}
			}
		}
		for (auto it = affiliation.cbegin(); it != affiliation.cend(); ++it)
		{
			string schoolNm = (*it).second;
			string personNm = (*it).first;
			boost::replace_all(schoolNm, "'", "''");
			boost::replace_all(personNm, "'", "''");
			W.exec("update person set affiliate = '" + schoolNm + "' where person_name = '" + personNm + "';");
		}*/

		//insert publication
		recordCnt = 0;
		clock_t publicationInsertBegin = clock();
		for (auto pubTypeIt = publicationTypes.cbegin(); pubTypeIt != publicationTypes.cend(); ++pubTypeIt)
		{
			string pubType = *pubTypeIt;
			for (xml_node<> *publication = root->first_node(&(pubType[0])); publication; publication = publication->next_sibling(&(pubType[0])))
			{
				xml_node<> *author = publication->first_node("author");
				xml_node<> *editor = publication->first_node("editor");
				string personNm, urlStr;
				if (!author && !editor)
				{
					continue;
				}
				else
				{
					++pubID;
					++recordCnt;
				}
				publicationSqlHolder += ("(" + to_string(pubID) + ",");
				for (auto pubAttri = publicationAttributes.cbegin(); pubAttri != publicationAttributes.cend(); ++pubAttri)
				{
					string attriNm = *pubAttri;
					xml_node<> *attriNode = publication->first_node(&(attriNm[0]));					
					if (attriNode)
					{
						string attriVal = attriNode->value();
						boost::replace_all(attriVal, "'", "''");
						publicationSqlHolder += ("'" + attriVal + "',");
					}
					else
					{
						publicationSqlHolder += "NULL,";
					}
				}
				publicationSqlHolder.pop_back();
				publicationSqlHolder += "),";
				if (recordCnt >= batchCnt)
				{
					publicationSqlHolder.pop_back();
					W.exec(publicationSqlHd + publicationSqlHolder + publicationSqlTl);
					publicationSqlHolder.clear();
					recordCnt = 0;
					break;/*insert a batch of records for demo purpose*/
				}
			}			
		}
		if (recordCnt > 0)
		{
			publicationSqlHolder.pop_back();
			W.exec(publicationSqlHd + publicationSqlHolder + publicationSqlTl);
			publicationSqlHolder.clear();
			recordCnt = 0;
		}
		clock_t publicationInsertEnd = clock();
		double publicationInsertTime = double(publicationInsertEnd - publicationInsertBegin) / CLOCKS_PER_SEC;
		

		//insert pub2person
		recordCnt = 0;
		pubID = 0;
		clock_t pub2personInsertBegin = clock();
		for (auto pubTypeIt = publicationTypes.cbegin(); pubTypeIt != publicationTypes.cend(); ++pubTypeIt)
		{
			string pubType = *pubTypeIt;
			for (xml_node<> *publication = root->first_node(&(pubType[0])); publication; publication = publication->next_sibling(&(pubType[0])))
			{
				xml_node<> *author = publication->first_node("author");
				xml_node<> *editor = publication->first_node("editor");
				string personNm, urlStr;
				if (!author && !editor)
				{
					continue;
				}
				else
				{
					++pubID;
					++recordCnt;
				}
				for (; author; author = author->next_sibling("author"))
				{
					personNm = author->value();
					boost::replace_all(personNm, "'", "''");
					pub2personSqlHolder += ("(" + to_string(pubID) + ",'" + pubType + "','" + personNm + "','author'),");
					++recordCnt;
				}
				for (; editor; editor = editor->next_sibling("editor"))
				{
					personNm = editor->value();
					boost::replace_all(personNm, "'", "''");
					pub2personSqlHolder += ("(" + to_string(pubID) + ",'" + pubType + "','" + personNm + "','editor'),");
					++recordCnt;
				}
				if (recordCnt >= batchCnt)
				{
					pub2personSqlHolder.pop_back();
					W.exec(pub2personSqlHd + pub2personSqlHolder + pub2personSqlTl);
					pub2personSqlHolder.clear();
					recordCnt = 0;
					break;/*insert a batch of records for demo purpose*/
				}
			}
		}
		if (recordCnt > 0)
		{
			pub2personSqlHolder.pop_back();
			W.exec(pub2personSqlHd + pub2personSqlHolder + pub2personSqlTl);
			pub2personSqlHolder.clear();
			recordCnt = 0;
		}
		clock_t pub2personInsertEnd = clock();
		double pub2personInsertTime = double(pub2personInsertEnd - pub2personInsertBegin) / CLOCKS_PER_SEC;
		
		/*for (xml_node<> *publication = root->first_node("www"); publication; publication = publication->next_sibling("www"))
		{
			recordCnt++;
			xml_node<> *author = publication->first_node("author");
			xml_node<> *editor = publication->first_node("editor");
			xml_node<> *url = publication->first_node("url");
			xml_node<> *school = publication->first_node("school");
			string urls, schools;
			if (url)
			{
				urls = url->value();
			}
			if (school)
			{
				schools = school->value();
				cout << schools;
			}
//			cout << "homepage: " << urls << "school: "<<schools<<", author: ";
			
			for (; author; author = author->next_sibling("author"))
			{
				string name = author->value();
		//		cout << name << ", ";
			}
			for (; editor; editor = editor->next_sibling("editor"))
			{
				if (publication->first_node("author"))
				{
					cout << "";
				}
			}
			if (author)
			{
				tmp = author->value();
				boost::replace_all(tmp, "'", "''");
				authorSqlHolder += "('";
				authorSqlHolder += tmp;
				authorSqlHolder += "'),";
				if (!isbn) continue;
			}
			else
			{
				continue;
			}
			bookSqlHolder += "(";
			for (auto i = 0; i < bookAttributes.size(); ++i)
			{
				xml_node<> *bookChild = book->first_node(&bookAttributes[i][0]);
				if (bookChild)
				{
					tmp = bookChild->value();
					boost::replace_all(tmp, "'", "''");
					bookSqlHolder += "'";
					bookSqlHolder += tmp;
					bookSqlHolder += "',";
				}
				else
				{
					bookSqlHolder += "NULL,";
				}
			}
			bookSqlHolder.pop_back();
			bookSqlHolder += "),";
			if (cnt == batchCnt)
			{
				if (authorSqlHolder.size() > 0)
				{
					authorSqlHolder.pop_back();
					W.exec(authorSqlHd + authorSqlHolder + authorSqlTl);
				}
				bookSqlHolder.pop_back();
				W.exec(bookSqlHd + bookSqlHolder + bookSqlTl);
				//W.commit();
				bookSqlHolder.clear();
				authorSqlHolder.clear();
				cnt = 1;
			}
			else
			{
				++cnt;
			}
		}
		if (authorSqlHolder.size() > 0)
		{
			authorSqlHolder.pop_back();
			W.exec(authorSqlHd + authorSqlHolder + authorSqlTl);
		}
		if (publicationSqlHolder.size() > 0)
		{	
			publicationSqlHolder.pop_back();
			W.exec(publicationSqlHd + publicationSqlHolder + publicationSqlTl);
			//W.commit();
		}
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		cout << "execution time for " << batchCnt << " records per batch: " << elapsed_secs << " seconds." << endl;
		cout << "Total records number: " << recordCnt << endl;
		getchar();*/
		cout << "person table insertion time: " << personInsertTime << " s." << endl;
		cout << "publication table insertion time: " << publicationInsertTime << " s." << endl;
		cout << "pub2person table insertion time: " << pub2personInsertTime << " s." << endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		getchar();
	}
	
	getchar();
	return 0;
}
