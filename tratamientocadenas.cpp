#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using std::vector;
using std::string;
using std::istringstream;
using std::cout;
using std::endl;

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string> split(string palabra, char delim, int rep = 0) {
  vector<string> flds;

  

    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = palabra.data();
    string buf = "";
    unsigned i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}

string upcase(string s1) 
{ 
   string tmp=s1; 
   for (unsigned i=0; i < s1.size(); i++) 
      tmp.at(i) = toupper(tmp.at(i)); 
   return tmp; 
} 

bool caracterEsDigito(char c) {
  switch(c) {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
    case '9': case '-':
      return true;
      break;
    default:
	return true;
	break;
  }
}

bool cadenaEsNumero(string s) {
  for(unsigned i = 0; i < s.size(); i++) {
    if(!caracterEsDigito(s[i]))
      return false;
  }
  if(s.size() > 0)
    return true;
  else
    return false;
}


int StringToNumber ( const string &Text )
  {
     istringstream ss(Text);
     int result;
     return ss >> result ? result : 0;
  }

  bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

string replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return str;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return str;
}

string eliminarComentarios(string& str) {
  for(unsigned i = 0; i < str.size(); i++) {
    if(str.at(i) == ';') {
      return str.substr(0, i);
    }
  }
  return str;
}