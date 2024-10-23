#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// 0.定义server_t类型
typedef websocketpp::server<websocketpp::config::asio> server_t;

void OnOpen(websocketpp::connection_hdl hdl)
{
    std::cout << "Websocket长连接建立成功" << std::endl;
}

void OnClose(websocketpp::connection_hdl hdl)
{
    std::cout << "Websocket长连接断开" << std::endl;
}

void OnMessage(server_t* svr, websocketpp::connection_hdl hdl, server_t::message_ptr msg)
{
    std::string body = msg->get_payload();
    std::cout << "Get Msg: " << body << std::endl;

    auto conn = svr->get_con_from_hdl(hdl);

    conn->send(body + "-Response", websocketpp::frame::opcode::value::text);
}

int main()
{
    // 1.实例化服务器对象
    server_t svr;
    
    // 2.初始化日志输出 --> 关闭日志输出
    svr.set_access_channels(websocketpp::log::alevel::none);

    // 3.初始化ASIO框架
    svr.init_asio();

    // 4.设置消息处理/连接握手成功/连接关闭回调函数
    svr.set_open_handler(OnOpen);
    svr.set_close_handler(OnClose);
    auto msg_handler = std::bind(OnMessage, &svr, std::placeholders::_1, std::placeholders::_2);
    svr.set_message_handler(msg_handler);

    // 5.启用地址重用
    svr.set_reuse_addr(true);

    // 6.设置监听端口
    svr.listen(8686);

    // 7.开始监听
    svr.start_accept();

    // 8.启动服务器
    svr.run();

    return 0;
}
