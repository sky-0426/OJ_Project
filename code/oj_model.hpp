#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
//#include <boost/algorithm/string.hpp>
#include <vector>

#include "tools.hpp"

struct Question
{
    //题目id
    std::string id_;
    //题目标题
    std::string title_;
    //难易程度
    std::string star_;
    //题目路径
    std::string path_;

    //题目的描述信息
    std::string desc_;
    //题目的预定义头
    std::string header_cpp_;
    //题目的尾部,包含测试用例和调用逻辑
    std::string tail_cpp_;
};


class OjModel
{
public:
    OjModel()
    {
        Load("./oj_data/oj_config.cfg");
    }
    ~OjModel()
    {

    }
    //从文件中获取题目信息
    bool Load(const std::string& filename)
    {
        std::ifstream file(filename.c_str());
        //如果文件没有被打开
        if(!file.is_open())
        {
            std::cout<<"config file open failed"<<std::endl;
            return false;
        }
        //1.打开文件成功
        //  1.1从文件中获取每一行信息
        //      1.1.1对于每一行信息，还需要获取题号、题目名称、难易程度、题目路径
        //      1.1.2保存在结构体当中
        //
        //2.把多个question,组织在map中
    
        std::string line;
        while(std::getline(file,line))
        {
            //boost::split
            std::vector<std::string> vec;
            StringUtil::Split(line,"\t",&vec);
            //is_any_of：支持多个字符作为分隔符
            //token_compress_off：是否将多个分割字符看成是一个
            //boost::split(vec,line,boost::is_any_of(" "),boost::token_compress_off);

            Question ques;
            ques.id_=vec[0];
            ques.title_=vec[1];
            ques.star_=vec[2];
            ques.path_=vec[3];

            std::string dir=vec[3];
            FileUtil::ReadFile(dir+"/desc.txt",&ques.desc_);
            FileUtil::ReadFile(dir+"/header.cpp",&ques.header_cpp_);
            FileUtil::ReadFile(dir+"/tail.cpp",&ques.tail_cpp_);

            ques_map_[ques.id_]=ques;
        }
        file.close();
        return true;
    }
    //获取试题列表
    bool GetAllQuestion(std::vector<Question>* questions)
    {
        //1.遍历无序的map,将试题信息返回给调用者
        //对于每一个试题，都是一个Question结构体对象
        for(const auto& kv:ques_map_)
        {
            questions->push_back(kv.second);
        }
        //排序
        std::sort(questions->begin(),questions->end(),[](const Question& l,const Question& r){
                  //比较Question当中的题目编号，按照升序规则
                  return std::stoi(l.id_)>std::stoi(r.id_);
                  });
        return true;
    }
    //获取单个试题
    //id：输入条件，查找题目的ID
    //ques：输出参数，将查找的结果返回给调用者
    bool GetOneQuestion(const std::string& id,Question* ques)
    {
        auto it=ques_map_.find(id);
        if(it==ques_map_.end())
        {
            return false;
        }
        *ques=it->second;
        return true;
    }
private:
    std::unordered_map<std::string,Question> ques_map_;
};
