#include <iostream>
#include <cstdlib>
#include <cstring>

inline std::string& GetFile(std::string& out, const std::string& str) {
	size_t pos = str.find_last_of("\\/");

	if (pos != std::string::npos)
		out = str.substr(pos + 1);
	else
		out = str;

	return out;
}

inline std::string& GetPath(std::string& out, const std::string& str)
{
	size_t pos = str.find_last_of("\\/");

	if (pos != std::string::npos) {
#ifdef _WIN32
		out = str.substr(0, pos) + '\\';
#else
		out = str.substr(0, pos) + '/';
#endif
	} else {
		out = "";
	}

	return out;
}

template <int size>
static char* CopyString(char (&dst)[size], const char* src)
{
#ifdef _MSC_VER
	strcpy_s(dst, size, src);
	return dst;
#else
	return strcpy(dst, src);
#endif
}

inline std::string& ToLower(std::string& out, const std::string& str)
{
	out.resize(str.size());

	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			out[i] = str[i] + 32;
		else
			out[i] = str[i];
	}

	return out;
}

inline std::string& GetExtension(std::string& out, const std::string& str)
{
	size_t pos = str.find_last_of(".");

	if (pos == std::string::npos) {
		out = "";
		return out;
	}

	out = str.substr(pos + 1, str.length());
	return ToLower(out, out);
}