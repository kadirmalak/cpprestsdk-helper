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

#pragma once

#include <cpprest/http_listener.h>

namespace cppresthelper {

	class BaseController {
		virtual void OnException(const web::http::http_request & request, const std::exception & ex) {}
	};

	struct RoutingEntry {
		utility::string_t url;
		web::http::method method = web::http::methods::GET;
		std::function<void(const web::http::http_request & request)> handler;
	};

	class BaseServer
	{
	public:
		virtual void ConfigureRouting() {}
		pplx::task<void> Start(const utility::string_t & endpoint) {
			using namespace web;
			using namespace web::http;
			using namespace web::http::experimental::listener;

			ConfigureRouting();

			_listener = http_listener(uri(endpoint));
			_listener.support([this](const http_request & request) {
				bool found = false;
				for (auto & entry : _routingEntries) {
					if (request.relative_uri().path() != entry.url || request.method() != entry.method) {
						continue;
					}
					entry.handler(request);
					found = true;
					break;
				}
				if (!found) {
					request.reply(status_codes::NotFound);
				}
			});

			return _listener.open();
		}
		pplx::task<void> Stop() { return _listener.close(); }
		const std::vector<RoutingEntry> & GetRoutingEntries() { return _routingEntries; }
		utility::string_t GetEndpoint() { return _listener.uri().to_string(); }
	protected:
		web::http::experimental::listener::http_listener _listener;
		std::vector<RoutingEntry> _routingEntries;
	};
}

#define CPPRESTHELPER_HANDLER(controller, action) [](const web::http::http_request & request) {\
	static_assert(std::is_base_of<cppresthelper::BaseController, controller>::value, "controller class must derive from 'public cppresthelper::BaseController'");\
	auto c = controller();\
	try {\
		c.action(request);\
	} catch (const std::exception & ex) {\
		c.OnException(request, ex);\
		request.reply(web::http::status_codes::InternalError);\
	} catch (...) {\
		c.OnException(request, std::exception("unhandled exception"));\
		request.reply(web::http::status_codes::InternalError);\
	}\
}
