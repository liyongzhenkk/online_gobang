#include <iostream>
#include <string>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

typedef websocketpp::server<websocketpp::config::asio> wsserver_t; // 异步输入输入的服务器

void http_callback(wsserver_t *srv, websocketpp::connection_hdl hdl)
{
    wsserver_t::connection_ptr conn = srv->get_con_from_hdl(hdl);
    std::cout << "body :" << conn->get_request_body();
    websocketpp::http::parser::request req = conn->get_request();
    std::cout << "method :" << req.get_method() << std::endl;
    std::cout << "uri :" << req.get_uri() << std::endl;
    std::string body = "<html><body><h1>Hello world</h1></body></html>";
    conn->set_body(body);
    conn->append_header("Content-Type", "text/html");
    conn->set_status(websocketpp::http::status_code::ok);
}
void wsopen_callback(wsserver_t *srv, websocketpp::connection_hdl hdl)
{
    std::cout << "websocket 握手成功" << std::endl;
}
void wsclose_callback(wsserver_t *srv, websocketpp::connection_hdl hdl)
{
    std::cout << "websocket 链接断开" << std::endl;
}
void wsmsg_callback(wsserver_t *srv, websocketpp::connection_hdl hdl, wsserver_t::message_ptr msg)
{
    wsserver_t::connection_ptr conn = srv->get_con_from_hdl(hdl);
    std::cout << "wsmsg : " << msg->get_payload() << std::endl;
    std::string rsp = "client say" + msg->get_payload();
    conn->send(rsp,websocketpp::frame::opcode::text);
}

int main()
{
    // 1.实例化server对象
    wsserver_t wssrv;
    // 2.设置日志等级
    wssrv.set_access_channels(websocketpp::log::alevel::none);
    // 3.初始化asio框架
    wssrv.init_asio();
    // 4.设置业务处理回掉函数
    wssrv.set_http_handler(std::bind(http_callback, &wssrv, std::placeholders::_1));
    wssrv.set_open_handler(std::bind(wsopen_callback, &wssrv, std::placeholders::_1));
    wssrv.set_close_handler(std::bind(wsclose_callback, &wssrv, std::placeholders::_1));
    wssrv.set_message_handler(std::bind(wsmsg_callback, &wssrv, std::placeholders::_1, std::placeholders::_2));
    // 5.设置 listen port
    wssrv.listen(8085);
    // 6.accept
    wssrv.start_accept();
    // 7.run
    wssrv.run();
    return 0;
}