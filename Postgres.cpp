#include "Postgres.h"
 
CPostgres::CPostgres()
{
	m_strConnInfo = "";
	m_pResult_Oper = new PGResultOper;
	m_dbConn = NULL;
	m_err_str = "";
	m_bConnected = false;
}
 
CPostgres::~CPostgres()
{
	if (m_pResult_Oper != nullptr)
	{
		delete m_pResult_Oper;
	}
}
 
// 建立连接 [10/21/2019 none]
bool CPostgres::DBConnect(std::string strConnInfo)
{
	m_strConnInfo = strConnInfo;
	bool bret = true;
	try {
		if (m_strConnInfo.empty())
		{
			m_err_str = "connect string is empty!";
			return false;
		}
 
		try
		{
			m_dbConn = new pqxx::connection(m_strConnInfo.c_str());
			if (m_dbConn == NULL)
			{
				m_err_str = "db connection ptr is null.";
				return false;
			}
		}
		catch (const std::exception &e)
		{
			m_err_str = "db connect unknown exception.";
			return false;
		}
		
		if (m_dbConn->is_open())
		{
			m_err_str = "Opened database successfully, dbname: " + std::string(m_dbConn->dbname());
		}
		else
		{
			bret = false;
			DisDBConnect();
		}
	}
	catch (const std::exception &e)
	{
		m_err_str = e.what();
		bret = false;
		DisDBConnect();
	}
	catch (...)
	{
		m_err_str = "unknown exception.";
		bret = false;
		DisDBConnect();
	}
 
	return bret;
}
 
// 断开连接 [10/21/2019 none]
void CPostgres::DisDBConnect()
{
	m_err_str.clear();
	if (m_dbConn == NULL)
	{
		m_err_str = "db connection ptr is null.";
		return;
	}
	m_dbConn->disconnect();
	if (m_dbConn != NULL)
	{
		delete m_dbConn;
		m_dbConn = NULL;
	}
}
 
 
 
// 重连 [10/21/2019 none]
bool CPostgres::Is_Connect()
{
	bool bret = false;
	m_err_str.clear();
	try
	{
		if (m_dbConn == nullptr)
		{
			m_err_str = "db connection ptr is null.";
			return false;
		}
		if (!m_dbConn->is_open())
		{
			m_bConnected = false;
			DisDBConnect();
			bret = DBConnect(m_strConnInfo);
		}
		else
		{
			bret = true;
			m_bConnected = true;
		}
 
	}
	catch (...)
	{
		m_err_str = "unknown exception.";
		m_bConnected = false;
		bret = false;
		DisDBConnect();
	}
	printf("Reconnect postgresql success \n");
	return bret;
}
 
 
 
// 提交 [10/21/2019 none]
bool CPostgres::ExecSql(const std::string& sql, bool bCommit)
{
	bool bret = true;
	m_err_str.clear();
	try {
		if (m_dbConn == nullptr)
		{
			m_err_str = "db connection ptr is null.";
			return false;
		}
		work worker(*m_dbConn);
		worker.exec(sql);
		worker.commit();
	}
	catch (const pqxx::pqxx_exception &pe)
	{
		m_err_str = pe.base().what();
		bret = false;
	}
	catch (const std::exception &e)
	{
		m_err_str = e.what();
		bret = false;
	}
	catch (...)
	{
		m_err_str = "unknown exception.";
		bret = false;
	}
	return bret;
}
 
//执行查询sql语句 [10/21/2019 none]
PGResultOper* CPostgres::Execute_Query(const std::string& sql)
{
	m_err_str.clear();
	try
	{
		if (m_dbConn == NULL)
		{
			m_err_str = "db connection ptr is null.";
			return nullptr;
		}
		nontransaction nts(*m_dbConn);				/* Create a non-transactional object. */
		pqxx::result res = nts.exec(sql);	/* Execute SQL query */
		m_res = res;
 
		map_Result results;	//查询结果集
		for (unsigned int row = 0; row < res.size(); row++)
		{
			std::map<std::string, std::string> pairs;		//map[行, map[字段名, 值] ]
			for (unsigned int col = 0; col < res.columns(); col++)
			{
				//printf("==map===clounm:%s, value:%s\n", res[row][col].name(), res[row][col].c_str());
				pairs.insert(std::make_pair(res[row][col].name(), res[row][col].c_str()));
				results[row] = pairs;
			}
		}
		m_pResult_Oper->Set_map_res(results, m_res);	//存储查询结果集
	}
	catch (const pqxx::pqxx_exception &pe)
	{
		m_err_str = pe.base().what();
		return nullptr;
	}
	catch (const std::exception &e)
	{
		m_err_str = e.what();
		return nullptr;
	}
	catch (...)
	{
		m_err_str = "unknown exception.";
		return nullptr;
	}
	return m_pResult_Oper;
}
 
 
/* --------------------------------------------------------------------------------------------- *
* PGResultOper
* --------------------------------------------------------------------------------------------- */
PGResultOper::PGResultOper()
{
	m_mapRes.clear();
	res_itor = m_mapRes.begin();
	m_bItor_first = true;
}
PGResultOper::~PGResultOper()
{
	m_mapRes.clear();
	res_itor = m_mapRes.begin();
}
 
// 根据字段名字取值 [10/22/2019 none]
const char * PGResultOper::Field(const char * col_name)
{
	if (res_itor == m_mapRes.end() || m_mapRes.empty())
	{
		return "";
	}
	return res_itor->second[col_name].c_str();
}
 
// 根据索引号字取值 [10/22/2019 none]
const char * PGResultOper::Field(unsigned int col_index)
{
	if (col_index >= res_itor->second.size() || m_mapRes.empty())
	{
		return "";
	}
	return m_pq_res[Row_Id()][col_index].c_str();
}
 
// 根据字段名字取值 [10/22/2019 none]
const char * PGResultOper::Get(const char * col_name)
{
	if (res_itor == m_mapRes.end() || m_mapRes.empty())
	{
		return "";
	}
	return res_itor->second[col_name].c_str();
}
 
// 根据索引号字段名字取值 [10/22/2019 none]
const char * PGResultOper::GetFieldName(unsigned int col_index)
{
	if (col_index >= res_itor->second.size() || m_mapRes.empty())
	{
		return "";
	}
	return m_pq_res[Row_Id()][col_index].name();
}
 
// 结果集大小(行数) [10/22/2019 none]
unsigned int PGResultOper::Row_Num()
{
	return (unsigned int)m_mapRes.size();
}
 
// 结果集列大小 [10/22/2019 none]
unsigned int PGResultOper::Column_Num()
{
	if (m_mapRes.empty())
	{
		return 0;
	}
	return res_itor->second.size();
}
 
// 判断结果集是否读完 [10/22/2019 none]
bool PGResultOper::IsEOF()
{
	if (m_mapRes.empty())
	{
		return true;
	}
	return (res_itor == m_mapRes.end());
}
 
// 读取下一行 [10/22/2019 none]
bool PGResultOper::Next()
{
	if (m_mapRes.empty())
	{
		return false;
	}
 
	if (res_itor != m_mapRes.end())
	{
		res_itor++;
		return true;
	}
	return false;
}
 
const char * PGResultOper::operator[](const char * col_name)
{
	return Get(col_name);
}
 
// 遍历结果集操作符 [10/22/2019 none]
bool PGResultOper::operator++(int)
{
	return Next();
}
 
void PGResultOper::Set_map_res(const map_Result& results, const pqxx::result& res)
{
	m_mapRes = results;
	res_itor = m_mapRes.begin();
	m_pq_res = res;
	m_bItor_first = true;
}
 
// 结果集行号,从1开始 [10/22/2019 none]
unsigned int PGResultOper::Row_Id()
{
	if (res_itor == m_mapRes.end())
	{
		return 0;
	}
	return res_itor->first;
}
 