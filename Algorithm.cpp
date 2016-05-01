#include <restclient-cpp/restclient.h>
#include <jeayeson/jeayeson.hpp>
#include <iostream>
#include <string>
int main() {
	char *buf; std::string arg;
	if ((buf = getenv("JSON_STRING")) != NULL) arg = buf;
	const std::string u{"http://oxfordhk.azure-api.net/academic/v1.0/evaluate?expr=Id=2140251882&count=10000&attributes=Id,AA.AuId,AA.AfId&subscription-key=f7cc29509a8443c5b3a5e56b0e38b5a6"};
	RestClient::Response r = RestClient::get(u.c_str());
	std::cout << r.body << std::endl;
	return 0;
}