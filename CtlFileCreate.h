/**********************************************************************
* 版权所有 (C)2015, Zhou Zhaoxiong。
*
* 文件名称：CtlFileCreate.h
* 文件标识：无
* 内容摘要：目录中文件的读取及控制文件的生成
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20151102
*
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// 数据类型重定义
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned int        UINT32;
typedef signed   int        INT32;
typedef unsigned char       BOOL;
 
// 参数类型
#define MML_INT8_TYPE       0
#define MML_INT16_TYPE      1
#define MML_INT32_TYPE      2
#define MML_STR_TYPE        3
 
#define  TRUE         (BOOL)1
#define  FALSE        (BOOL)0

// 字段最大长度
#define MAX_RET_BUF_LEN     1024

// 源文件字段结构体
typedef struct
{
    UINT8 szSrcNumber[50];
	UINT8 szDstNumber[50];
	UINT8 szDataStartTime[50]; 
    UINT8 szDataEndTime[50]; 
} T_SrcFileContent;

// 函数声明
void Sleep(UINT32 iCountMs);
void ReadCtlFile(void);
void ReadSrcFileAndWriteCtlFile(void);
void GetSrcFileContentAndWriteCtlFile(UINT8 *pszSrcFileName);
void GetSrcFileFieldValue(UINT8 *pszContentLine, T_SrcFileContent *ptSrcFileContent);
void GetCtlFileContentAndWrite(T_SrcFileContent *ptSrcFileContent, UINT8 *pszContentBuffer);
BOOL GetValueFromStr(UINT16 iSerialNum, UINT8 iContentType, UINT8 *pSourceStr, UINT8 *pDstStr, UINT8 cIsolater, UINT32 iDstStrSize);
void RemoveLineEnd(UINT8 *pszStr);
void WriteToCtlFile(UINT8 *pszContentLine);
