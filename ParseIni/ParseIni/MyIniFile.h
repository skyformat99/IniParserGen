#include <string>
#include <map>
#include <fstream>
#include <stdexcept>
#include "minicsv.h"
#include "Color.h"

class MyIniFile
{
private:
	std::map<std::string, std::string> m_NameValueMap;
	std::string m_File;
public:
	bool ParseFile(const std::string& file)
	{
		m_NameValueMap.clear();
		csv::ifstream is(file.c_str());
		is.set_delimiter('=', "$$");
		is.enable_terminate_on_blank_line(false);
		if (is.is_open())
		{
			m_File = file;
			while (is.read_line())
			{
				std::string name;
				std::string value;
				is >> name;
				value = is.get_rest_of_line();
				m_NameValueMap[csv::trim(name, " \t")] = csv::trim(value, " \t");
			}
			is.close();
			return Validate();
		}
		else
		{
			std::ostringstream oss;
			oss << "File cannot be opened:" << file;
			throw std::runtime_error(oss.str().c_str());
		}
		return true;
	}
private:
	bool WriteFile(const std::string& key, const std::string& val)
	{
		std::vector<std::pair<std::string, std::string> > vec;
		csv::ifstream is(m_File.c_str());
		is.set_delimiter('=', "$$");
		is.enable_terminate_on_blank_line(false);
		if (is.is_open())
		{
			bool found=false;
			while (is.read_line())
			{
				std::string name;
				std::string value;
				is >> name;
				value = is.get_rest_of_line();

				if (csv::trim(name, " \t") == key)
				{
					value = val;
					found = true;
				}
				vec.push_back(std::make_pair(name, value));
			}
			if(!found)
			{
				vec.push_back(std::make_pair(key, val));
			}
			is.close();

			csv::ofstream os(m_File.c_str());
			os.set_delimiter('=', "$$");
			if (os.is_open())
			{
				for (size_t i = 0; i<vec.size(); ++i)
				{
					os << vec[i].first << vec[i].second << NEWLINE;
				}
				os.flush();
				os.close();
			}
			else
			{
				std::ostringstream oss;
				oss << "File cannot be opened for writing:" << m_File;
				throw std::runtime_error(oss.str().c_str());
			}
		}
		else
		{
			std::ostringstream oss;
			oss << "File cannot be opened for reading:" << m_File;
			throw std::runtime_error(oss.str().c_str());
		}
		return true;
	}
public:
	bool Exists(const std::string& name)
	{
		typedef std::map<std::string, std::string> Map;
		typedef Map::const_iterator MapConstIter;
		MapConstIter it = m_NameValueMap.find(name);
		return it != m_NameValueMap.end();
	}
	// Getter member functions
	std::string StartDate()
	{
		if(!Exists("StartDate"))
		{
			throw std::runtime_error("StartDate does not exist");
		}
		std::string val;
		std::istringstream iss(m_NameValueMap["StartDate"]);
		iss >> val;
		return val;
	}
	std::string EndDate()
	{
		if(!Exists("EndDate"))
		{
			throw std::runtime_error("EndDate does not exist");
		}
		std::string val;
		std::istringstream iss(m_NameValueMap["EndDate"]);
		iss >> val;
		return val;
	}
	int Alpha()
	{
		if(!Exists("Alpha"))
		{
			throw std::runtime_error("Alpha does not exist");
		}
		int val;
		std::istringstream iss(m_NameValueMap["Alpha"]);
		iss >> val;
		return val;
	}
	bool CheckFolder()
	{
		if(!Exists("CheckFolder"))
		{
			throw std::runtime_error("CheckFolder does not exist");
		}
		bool val = false;
		std::string s(m_NameValueMap["CheckFolder"]);
		if(s=="Y"||s=="1"||s=="true") val=true;
		else if(s=="N"||s=="0"||s=="false") val=false;
		return val;
	}
	Color TintedColor()
	{
		if(!Exists("TintedColor"))
		{
			throw std::runtime_error("TintedColor does not exist");
		}
		Color val;
		std::istringstream iss(m_NameValueMap["TintedColor"]);
		iss >> val;
		return val;
	}
	// GetSafe* member functions
	std::string GetSafeStartDate(std::string default_val)
	{
		if(Exists("StartDate"))
			return StartDate();
		else
			return default_val;
	}
	std::string GetSafeEndDate(std::string default_val)
	{
		if(Exists("EndDate"))
			return EndDate();
		else
			return default_val;
	}
	int GetSafeAlpha(int default_val)
	{
		if(Exists("Alpha"))
			return Alpha();
		else
			return default_val;
	}
	bool GetSafeCheckFolder(bool default_val)
	{
		if(Exists("CheckFolder"))
			return CheckFolder();
		else
			return default_val;
	}
	Color GetSafeTintedColor(Color default_val)
	{
		if(Exists("TintedColor"))
			return TintedColor();
		else
			return default_val;
	}
	// IsValid* member functions
	bool IsValidStartDate()
	{
		bool ret = false;
		try
		{
			ret = true;
		}
		catch(std::exception&)
		{
		}
		return ret;
	}
	bool IsValidEndDate()
	{
		bool ret = false;
		try
		{
			ret = Exists("StartDate") && EndDate() >= StartDate();
		}
		catch(std::exception&)
		{
		}
		return ret;
	}
	bool IsValidAlpha()
	{
		bool ret = false;
		try
		{
			ret = Alpha() >= 0 && Alpha() <= 255;
		}
		catch(std::exception&)
		{
		}
		return ret;
	}
	bool IsValidCheckFolder()
	{
		bool ret = false;
		try
		{
			ret = true;
		}
		catch(std::exception&)
		{
		}
		std::string s(m_NameValueMap["CheckFolder"]);
		return ret&&(s=="Y"||s=="1"||s=="true"||s=="N"||s=="0"||s=="false");
	}
	bool IsValidTintedColor()
	{
		bool ret = false;
		try
		{
			ret = true;
		}
		catch(std::exception&)
		{
		}
		return ret;
	}
	// Setter member functions
	bool SetStartDate(std::string val)
	{
		std::ostringstream oss;
		oss << val;
		std::string str_val = oss.str(); 
		if (str_val != m_NameValueMap["StartDate"])
		{
			m_NameValueMap["StartDate"] = str_val;
		}
		return WriteFile("StartDate", str_val);
	}
	bool SetEndDate(std::string val)
	{
		std::ostringstream oss;
		oss << val;
		std::string str_val = oss.str(); 
		if (str_val != m_NameValueMap["EndDate"])
		{
			m_NameValueMap["EndDate"] = str_val;
		}
		return WriteFile("EndDate", str_val);
	}
	bool SetAlpha(int val)
	{
		std::ostringstream oss;
		oss << val;
		std::string str_val = oss.str(); 
		if (str_val != m_NameValueMap["Alpha"])
		{
			m_NameValueMap["Alpha"] = str_val;
		}
		return WriteFile("Alpha", str_val);
	}
	bool SetCheckFolder(bool val)
	{
		std::ostringstream oss;
		oss << std::boolalpha << val;
		std::string str_val = oss.str(); 
		if (str_val != m_NameValueMap["CheckFolder"])
		{
			m_NameValueMap["CheckFolder"] = str_val;
		}
		return WriteFile("CheckFolder", str_val);
	}
	bool SetTintedColor(Color val)
	{
		std::ostringstream oss;
		oss << val;
		std::string str_val = oss.str(); 
		if (str_val != m_NameValueMap["TintedColor"])
		{
			m_NameValueMap["TintedColor"] = str_val;
		}
		return WriteFile("TintedColor", str_val);
	}
	// Validate member function
	bool Validate()
	{
		std::ostringstream oss;
		if(!IsValidStartDate())
		{
			oss << "StartDate validation fails!" << std::endl;
		}
		if(!IsValidEndDate())
		{
			oss << "EndDate validation fails!" << std::endl;
		}
		if(!IsValidAlpha())
		{
			oss << "Alpha validation fails!" << std::endl;
		}
		if(!IsValidCheckFolder())
		{
			oss << "CheckFolder validation fails!" << std::endl;
		}
		if(!IsValidTintedColor())
		{
			oss << "TintedColor validation fails!" << std::endl;
		}
		std::string error = oss.str();
		if (!error.empty())
		{
			throw std::runtime_error(error.c_str());
		}
		return true;
	}
};
