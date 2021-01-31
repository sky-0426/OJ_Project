#include "httplib.h"
#include <stdio.h>

void func(const httplib::Request& req,httplib::Response& rsp)
{
    rsp.set_content("<html>66</html>",15,"text/html");
    printf("recv abc\n");
}

int main()
{
    //创建httplib当中的server对象，使用该类对象，模拟创建一个http服务器
    httplib::Server svr;

    svr.Get("/abc",func);

    svr.listen("0.0.0.0",18989);
    return 0;
}

