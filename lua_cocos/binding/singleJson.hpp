#ifndef SINGLEJSON_HPP
#define SINGLEJSON_HPP

#include <map>
using namespace std;

struct SingleJson{
	map<string, int> _ints;
	map<string, string> _strings;
	map<string, float> _floats;
	map<string, bool> _bools;
	string stringify(){
		string jsonTxt = "{";
		map<string, int>::iterator iterInt;
		iterInt = _ints.begin();
		while (iterInt != _ints.end()) {
			if (jsonTxt != "{")
				jsonTxt += ",";
			jsonTxt += setString(iterInt->first) + ":" + setInt(iterInt->second);
			iterInt++;
		}

		map<string, string>::iterator iterString;
		iterString = _strings.begin();
		while (iterString != _strings.end()) {
			if (jsonTxt != "{")
				jsonTxt += ",";
			jsonTxt += setString(iterString->first) + ":" + setString(iterString->second);
			iterString++;
		}

		map<string, float>::iterator iterFloat;
		iterFloat = _floats.begin();
		while (iterFloat != _floats.end()) {
			if (jsonTxt != "{")
				jsonTxt += ",";
			jsonTxt += setString(iterFloat->first) + ":" + setFloat(iterFloat->second);
			iterFloat++;
		}

		map<string, bool>::iterator iterBool;
		iterBool = _bools.begin();
		while (iterBool != _bools.end()) {
			if (jsonTxt != "{")
				jsonTxt += ",";
			jsonTxt += setString(iterBool->first) + ":" + setBool(iterBool->second);
			iterBool++;
		}
		
		jsonTxt += "}";

		return jsonTxt;
	}
private:
	string setString(string str){
		return "\"" + str + "\"";
	}
	string setInt(int aInt){
		char chars[32];
		sprintf(chars, "%d", aInt);
		return chars;
	}
	string setFloat(float afloat){
		char chars[32];
		sprintf(chars, "%f", afloat);
		return chars;
	}
	string setBool(bool abool){
		string str = abool ? "true" : "false";
		return str;
	}
};

#endif