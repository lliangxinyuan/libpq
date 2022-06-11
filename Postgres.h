#ifndef __POSTGRES__
#define __POSTGRES__
#include <iostream>
#include <string>
#include "pqxx/pqxx"
#include <map>
 
using namespace std;
using namespace pqxx;
 
typedef std::map<int, std::map<std::string, std::string> > map_Result;
class PGResultOper;
 
class CPostgres
{
public:
	CPostgres();
	
	~CPostgres();
 
	// 建立连接 [10/21/2019 none]
	bool DBConnect(string connstring = "");
 
	// 断开连接 [10/21/2019 none]
	void DisDBConnect();
 
	// 执行增删改sql语句 [10/21/2019 none]
	bool ExecSql(const std::string& sql, bool bCommit = true);
 
	//执行查询sql语句 [10/21/2019 none]
	PGResultOper* Execute_Query(const std::string& sql);
private:
	// 重连 [10/21/2019 none]
	bool Is_Connect();
private:
	PGResultOper*		m_pResult_Oper;	//结果集类
	pqxx::connection*		m_dbConn; 
	pqxx::result			m_res;
	std::string				m_strConnInfo;
	std::string				m_err_str;
	bool					m_bConnected;
};
 
// 结果集类
class PGResultOper
{
public:
	PGResultOper();
	~PGResultOper();
 
public:
	// 根据字段名字取值 [10/22/2019 none]
	const char* Field(const char* col_name);
 
	// 根据索引号字取值 [10/22/2019 none]
	const char* Field(unsigned int col_index);
 
	// 根据字段名字取值 [10/22/2019 none]
	const char* Get(const char* col_name);
 
	// 根据索引号字段名字取值 [10/22/2019 none]
	const char* GetFieldName(unsigned int col_index);
 
	// 结果集大小(行数) [10/22/2019 none]
	unsigned int Row_Num();
	
	// 结果集列大小 [10/22/2019 none]
	unsigned int Column_Num();
 
	// 结果集行号,从1开始 [10/22/2019 none]
	unsigned int Row_Id();
 
	// 判断结果集是否读完 [10/22/2019 none]
	bool IsEOF();
 
	// 读取下一行 [10/22/2019 none]
	bool Next();
 
	//根据字段名字取值, 运算符重载
	const char* operator[](const char* col_name);
 
	// 遍历结果集操作符 [10/22/2019 none]
	bool operator++(int);
 
	// 存储查询结果集 [10/22/2019 none]
	void Set_map_res(const map_Result& results, const pqxx::result& res);
 
private:
	bool					m_bItor_first;
	map_Result::iterator	res_itor;
	map_Result				m_mapRes;
	pqxx::result			m_pq_res;
};
 
#endif // !__POSTGRES__