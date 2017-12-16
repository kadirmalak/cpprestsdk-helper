# cpprestsdk-helper
Header only library for structuring cpprestsdk http listener projects (routing/controllers etc...)

# usage
- include header
- create controller
- create server and routing entries
- run server

# demo

## sample controller

~~~C++
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
~~~

## sample server and routing
~~~C++
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
~~~

## run server
~~~C++
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
~~~
