#include <iostream>
#include <cstdio>

#include <jsoncpp/json/json.h>

#include "httplib.h"

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "compile.hpp"
#include "tools.hpp"
int main()
{
    using namespace httplib;
    OjModel model;
    //1、初始化httplib库的server对象
    Server svr;

    //2、提供三个接口，分别处理三个请求
    //2.1获取整个试题列表
    svr.Get("/all_questions",[&model](const Request& req, Response& resp){
            //1.返回试题列表
            std::vector<Question> questions;
            model.GetAllQuestion(&questions);

            for(size_t i=0;i<questions.size();i++)
            {
                std::cout<<questions[i].title_<<std::endl;
            }
            std::string html;
            OjView::DrawAllQuestions(questions,&html);


            resp.set_content(html,"text/html");
            });
    //2.2返回单个试题
    //  正则表达式
    //  \d+：表示多位数字
    svr.Get(R"(/question/(\d+))",[&model](const Request& req,Response& resp){
            //1.获取URL当中关于试题的数字
            Question ques;
            model.GetOneQuestion(req.matches[1].str(),&ques);

            //2.渲染模板的html
            std::string html;
            OjView::DrawOneQuestion(ques,&html);
            resp.set_content(html,"text/html");
            });
    //2.3编译运行
    svr.Post(R"(/compile/(\d+))",[&model](const Request& req,Response& resp){
            //1.获取试题编号
             Question ques;
             model.GetOneQuestion(req.matches[1].str(),&ques);
             //main函数调用+测试用例
             
             //Post方法在提交代码的时候，是经过encode的，要想正常获取浏览器提交的编码
             //需要进行decode,然后和tail.cpp进行合并，产生待编译的源码
             //Key:value ——> code:代码
             std::unordered_map<std::string,std::string> body_kv;
             UrlUtil::PraseBody(req.body,&body_kv);

             std::string user_code=body_kv["code"];
             //2.构造json对象，交给编译运行模块
             Json::Value req_json;
             req_json["code"]=user_code+ques.tail_cpp_;
             req_json["stdin"]="";
             std::cout<<req_json["code"].asCString()<<std::endl;
                
             Json::Value resp_json;
             Compiler::CompileAndRun(req_json,&resp_json);

             //获取的接口都在resp_json当中
             std::string err_no=resp_json["errorno"].asString();
             std::string case_result=resp_json["stdout"].asString();
             std::string reason=resp_json["reason"].asString();

             std::string html;
             OjView::DrawCaseResult(err_no,case_result,reason,&html);

             resp.set_content(html,"text/html");
             });

    LOG(INFO,"listen_port")<<": 17878"<<std::endl;
    svr.set_base_dir("./www");
    svr.listen("0.0.0.0",17878);
    return 0;
}
