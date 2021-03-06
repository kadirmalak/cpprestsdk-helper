//MIT License
//
//Copyright(c) 2017 kadir malak (kadir.malak@gmail.com)
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "stdafx.h"

#include "cpprest_helper.hpp"

class SampleController : public cppresthelper::BaseController {
public:

	SampleController()
	{
		std::wcout << U("controller created!") << std::endl;
	}

	void OnException(const web::http::http_request & request, const std::exception & ex) {
		std::wcout << U("exception occurred in SampleController: ")
			<< request.relative_uri().to_string() << std::endl;
		std::wcout << U("exception: ") << ex.what() << std::endl;
	}

	void Method1(const web::http::http_request & request) {
		using namespace web;
		using namespace web::http;
		auto response = json::value::object();
		response[U("value")] = json::value::string(U("json response from method 1"));
		request.reply(status_codes::OK, response);
	}

	void Method2(const web::http::http_request & request) {
		using namespace web::http;
		request.reply(status_codes::OK, U("string response from method 2"));
	}

	void FaultyMethod(const web::http::http_request & request) {
		throw std::exception("some error occurred!");
	}
};

class SampleServer : public cppresthelper::BaseServer
{
public:
	void ConfigureRouting() {

		using namespace cppresthelper;

		_routingEntries.push_back(RoutingEntry{
			U("/sample/method1"),
			web::http::methods::GET,
			CPPRESTHELPER_HANDLER(SampleController, Method1)
			});

		_routingEntries.push_back(RoutingEntry{
			U("/sample/method2"),
			web::http::methods::POST,
			CPPRESTHELPER_HANDLER(SampleController, Method2)
			});

		_routingEntries.push_back(RoutingEntry{
			U("/sample/faulty-method"),
			web::http::methods::GET,
			CPPRESTHELPER_HANDLER(SampleController, FaultyMethod)
			});
	}
};

int main()
{
	SampleServer server;

	server.Start(U("http://localhost:9876/api")).then([&server]() {
		std::wcout << U("server started listening...") << server.GetEndpoint() << std::endl;

		std::wcout << U("available urls:") << std::endl;
		for (auto & entry : server.GetRoutingEntries()) {
			std::wcout << U("[" << entry.method << "] ") << entry.url << std::endl;
		}

		std::wcout << U("press any key to stop server...") << std::endl;
	});

	std::cin.get();

	server.Stop().wait();
	std::wcout << U("server stopped...") << std::endl;

	return 0;
}

