#include <iostream>
#include <pqxx/pqxx> 
#include "Postgres.h"
#include <string.h>
 
using namespace std;
using namespace pqxx;
 
// g++ *.cpp  -std=c++11 -lpqxx -lpq  -I/usr/local/include -o main -w
int main(int argc, char* argv[])
{
	CPostgres *postgres = new CPostgres();
	//host=172.20.32.241 dbname=dnps user=dnps password=dnps port=5432 connect_timeout=5
	if (!postgres->DBConnect("dbname=postgres user=postgres password=njmkx2019."))
	{
		printf("postgresql connect failed\n");
		return -1;
	}
	printf("postgresql connect success\n");
 
	char szSql[256] = { 0 };
	memset(szSql, 0, 256);
	sprintf(szSql, "insert into turns (turn) values ('%s')", "10086");
 
	// insert
	printf("测试插入操作\n");
	for (int i = 0; i < 4; ++i)
	{
		if(!postgres->ExecSql(szSql))	//insert into "testpg"("xh", "sbbh", "sbmc") values ('1', '1', '1')
		{
			printf("Insert data failed\n");
			continue;
		}
		printf("Insert data success \n");
	}
	
	//测试查询操作
	printf("测试查询操作\n");
	PGResultOper* results = NULL;
	results = postgres->Execute_Query("select * from turns where turn='10086'");
	if (results == nullptr)
	{
		printf("querry data failed\n");
		return -1;
	}
	while (!results->IsEOF())
	{
		string str_name = results->GetFieldName(0);
		string turnTemp = results->Field("turn");
		int turn = atoi(turnTemp.c_str());
		printf("str_name:%s, turn:%d\n", str_name.c_str(), turn);
		results->Next();
	}
	//测试更新操作
	printf("测试更新操作\n");
	if (!postgres->ExecSql("update turns set turn = '101' where turn = '10086';"))
	{
		printf("Update data failed\n");
	}
	printf("Update data success \n");
 
	//测试更新后查询操作
	printf("测试更新后查询操作\n");
	results = NULL;
	results = postgres->Execute_Query("select * from turns where turn='101'");
	if (results == nullptr)
	{
		printf("querry data failed\n");
		return -1;
	}
	while (!results->IsEOF())
	{
		string str_name = results->GetFieldName(0);
		string turnTemp = results->Field("turn");
		int turn = atoi(turnTemp.c_str());
		printf("str_name:%s, turn:%d\n", str_name.c_str(), turn);
		results->Next();
	}
 
	postgres->DisDBConnect();
	delete postgres;
	return 0;
}