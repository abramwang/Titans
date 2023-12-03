#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <iostream>
#include <string>
#include <map>
using namespace std;

class Value
{
public:
    Value();
    Value(bool value);
    Value(int value);
    Value(double value);
    Value(const char* value);
    Value(const string& value);

    Value& operator = (bool value);
    Value& operator = (int value);
    Value& operator = (double value);
    Value& operator = (const char* value);
    Value& operator = (const string& value);

    operator bool();
    operator int();
    operator double();
    operator string();

private:
    string m_value;

};


typedef std::map<string, Value> Section;
class IniFile
{
public:
    IniFile();
    bool load(const string& filename);
    Value& get(const string& section, const string& key);
    void set(const string& section, const string& key, const Value& Value);
    bool has(const string& section);
    bool has(const string& section, const string& key);
    void remove(const string& section, const string& key);
    void remove(const string& section);
    void clear();
    void show();
    bool save(const string& filename);


    inline Section& operator [] (const string& section)
    {
        return m_section[section];
    } 

private:
    // 去除字符串的 前后空格 以及 回车换行符
    string& trim(string& s);
    string str();

private:
    string m_filename; 
    std::map<string, Section> m_section;
};

#endif
