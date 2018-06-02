#include <iostream>
#include <fstream>
#include <regex>

#include <set>
#include <map>

#include<ctime>
using namespace std;
int transnum = 9835;
set <string> transaction[9835];
//set <string> iterm;
//int countiterm[169];
int min_sup=700;
double min_conf = 0.2;
int Apriori(map<set<string>,int> LkSet,int k,map<set<string>,int> iterm);
void displaySet(set<string> strset);
void displayMap(map<set<string>,int> Ck);
void getRules(map <set<string>,int> iterm,map<set<string>,int> LkSetplus);
int main()
{
    clock_t startc,finish;
    startc = clock();
    map <set<string>,int> iterm;
    ifstream fin("Groceries.csv");
    if(fin.is_open() == false)
    {
        cout<<"file open error\n";
        return 0;
    }
    //string line = "\"9833\",\"{frank fur,tersrol/lsbunssoda}{12345 4567,UTF-milk,ttt (seed)}{black bread,vegtable}\"";
    string line;
    getline(fin,line);
    
    // set <string> iterm;
    //regex reg("\\{[\\w,/\\-() ]*\\}");
    regex reg("\\{[^\\}]*\\}");
    regex reg2(",");
    smatch r1;
    
    string ::const_iterator start = line.begin();
    string ::const_iterator end = line.end();
    int count = 0;
    while(regex_search(start,end,r1,reg))
    {
        //cout<<r1[0]<<endl;
        string temp = r1.str();
        temp = temp.substr(1,temp.length()-2);
        //cout<<temp<<endl;
        const char *p = temp.c_str();
        char p2[1000];
        strcpy(p2,p);
        //cout<<p2<<endl;
        char *p3 =strtok(p2,",");
        //cout<<"p3:"<<p3<<endl;
        string addtoset = p3;
        transaction[count].insert(addtoset);
        set<string> temp2;
        temp2.insert(addtoset);
        iterm.insert(pair<set<string>,int>(temp2,0));
        while((p3=strtok(NULL,",")) != NULL)
        {
            //cout<<"p3:"<<p3<<endl;
            addtoset = p3;
            transaction[count].insert(addtoset);
            
            set<string> temp3;
            temp3.insert(addtoset);
            iterm.insert(pair<set<string>,int>(temp3,0));
        }
        start = r1[0].second;
        //cout<<endl;
        count++;
    }
    cout<<count<<endl;
    cout<<iterm.size()<<endl;
    //cout<<transaction[0].size()<<endl;
    //cout<<transaction[1].size()<<endl;
    //上面打开文件扫描了一遍 得到了事务集D和C1
    cout<<endl;
    //下面得到C1计数得到L1
    for(int i=0;i<transnum;i++)
    {
        set<string>::iterator it = transaction[i].begin();
        for(;it!=transaction[i].end();it++)
        {
            string temp= it->c_str();
            set<string> temp2;
            temp2.insert(temp);
            iterm[temp2]++;
        }
    }
    
    map<set<string>,int> L1;
    
    map<set<string>,int> ::iterator mapit = iterm.begin();
    int c=0;
    for(;mapit != iterm.end();mapit++)
    {
        //输出大于最小支持度的1项集  并存入L1
        if(mapit->second > min_sup)
        {
            //先输出所有项集
            //            set<string> ::iterator sit = mapit->first.begin();
            //            for(;sit !=  mapit->first.end();sit++)
            //            {
            //                cout<<*sit<<",";
            //            }
            //            cout<<"->"<<mapit->second<<endl;
            L1.insert(*mapit);
            c++;
        }
    }
    cout<<"strat:\n";
    Apriori(L1,1,iterm);
    
    finish = clock();
    double totaltime=(double)(finish-startc)/CLOCKS_PER_SEC;
    cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
    return 0;
}
set<string> getSingle(map<set<string>,int> Ck)
{
    set<string> result;
    map<set<string>,int> ::iterator it =Ck.begin();
    for(;it != Ck.end();it++)
    {
        set<string> temp = it->first;
        set<string> ::iterator setit = temp.begin();
        for(;setit != temp.end();setit++)
        {
            result.insert(*setit);
        }
    }
    return result;
}
bool eraseOneInCk(set<string>temp,map<set<string>,int> Ck)
{
    set<string> ::iterator it = temp.begin();
    for(;it != temp.end(); it++)
    {
        set<string> a = temp;
        //    cout<<"a.size0:"<<a.size()<<"temp.size0:"<<temp.size()<<endl;
        string stemp = *it;  //调了半天才发现的bug  不能直接a.erase(it),这样不对，结果很迷 a.size()是减少1 但是输出还是输出两个
        a.erase(stemp);
        //     cout<<"a.size:"<<a.size()<<"temp.size:"<<temp.size()<<endl;
        //     displaySet(a);
        if(Ck.find(a) == Ck.end())
        {
            return false;
        }
    }
    return true;
}
map<set<string>,int> CktoCkplus(map<set<string>,int> Ck) //从Ck 返回Ck+1
{
    map<set<string>,int> result;
    map<set<string>,int> ::iterator it =Ck.begin();
    
    //得到所有set集中项的集合
    set<string> singleSet = getSingle(Ck);
    cout<<"--------------------------------------"<<endl;
    // displaySet(singleSet);
    cout<<endl<<singleSet.size()<<endl;
    for(;it != Ck.end();it++)
    {
        //对每一个set,找到不在这个set集里的个数
        set<string> temp = it->first;
        
        set<string> ::iterator setit = singleSet.begin();
        for(;setit != singleSet.end();setit++)
        {
            //对单个项集中的一个，如果不在 加入
            if(temp.find(*setit) == temp.end())
            {
                //加入后 判断 去除一个后是否仍然在Ck中
                set<string> param= temp;
                param.insert(*setit);
                //displaySet(param);
                bool J =eraseOneInCk(param,Ck);
                if(J == true)
                {
                    //每一个子集都在原来的项集里面
                    pair<set<string>,int> newre(param,0);
                    result.insert(newre);
                }
            }
        }
    }
    return result;
}
void displaySet(set<string> strset)
{
    set<string>::iterator sit = strset.begin();
    for(;sit != strset.end() ;sit++)
    {
        cout<<*sit<<",";
    }
}
void displayMap(map<set<string>,int> Ck)
{
    map<set<string>,int> ::iterator mapit = Ck.begin();
    for(;mapit != Ck.end();mapit++)
    {
        displaySet(mapit->first);
        cout<<"->";
        cout<<mapit->second;
        cout<<endl;
    }
}
bool isSubSet(set<string> s1,set<string> s2)
{
    set<string> ::iterator sit = s1.begin();
    for(;sit != s1.end();sit++)
    {
        if(s2.find(*sit) == s2.end())
        {
            return false;
        }
    }
    return true;
}
int Apriori(map<set<string>,int> LkSet,int k,map<set<string>,int> iterm)
{
    while(LkSet.size() > 1)
    {
        map<set<string>,int> newC = CktoCkplus(LkSet);
        k++;
        cout<<"C"<<k<<":"<<newC.size()<<endl;
        //select Lk+1
        
        map<set<string>,int> LkSetplus;
        map<set<string>,int> ::iterator it = newC.begin();
        for(;it != newC.end();it++)
        {
            //遍历一遍事务集
            for(int i=0;i<transnum;i++)
            {
                if(isSubSet(it->first, transaction[i]) == true)
                {
                    it->second++;
                }
            }
            
            if(it->second >min_sup)
            {
                LkSetplus.insert(pair<set<string>,int>(it->first,it->second));
            }
        }
        cout<<"---------------------------------"<<endl;
        cout<<"L2:"<<LkSetplus.size()<<endl;
        //displayMap(LkSetplus);
        getRules(iterm,LkSetplus);
        LkSet = LkSetplus;
    }
    
    return 0;
}
void getRules(map <set<string>,int> iterm,map<set<string>,int> LkSetplus)// 解析Lk,获得关联规则
{
    map <set<string>,int> ::iterator it1 = LkSetplus.begin();
    for(;it1 != LkSetplus.end() ;it1++)
    {
        int fz = it1->second;
        //cout<<"fz"<<fz<<endl;
        set<string> ::iterator setit = it1->first.begin();
        
        for(;setit != it1->first.end(); setit ++)
        {
            set<string> temp;
            temp.insert(*setit);
            int fm = iterm[temp];
            //cout<<"fm"<<fm<<endl;
            //cout<<"rules:"<<*setit<<"==>";
            //----------get str1
            string str1 = " ";
            set<string> ::iterator it2 = it1->first.begin();
            for(;it2 != it1->first.end();it2++)
            {
                if(*it2 != *setit)
                {
                    str1+=*it2;
                    str1+=",";
                }
            }
            //-----------------------------
            double ko = double(fz)/double(fm);
            if(ko > min_conf)
                cout<<"rules:"<<*setit<<"==>"<<str1<<" conf:"<<ko<<endl;
            
        }
    }
}

