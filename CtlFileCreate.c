/**********************************************************************
* 版权所有 (C)2015, Zhou Zhaoxiong。
*
* 文件名称：CtlFileCreate.c
* 文件标识：无
* 内容摘要：目录中文件的读取及控制文件的生成
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20151102
*
**********************************************************************/
#include "CtlFileCreate.h"

// 全局变量
UINT8  g_szSourceDir[500]    = {0};         // 需扫描的源目录
UINT8  g_szCtlFileDir[500]   = {0};         // 生成的控制文件的存放目录
UINT8  g_szSourceBakDir[500] = {0};         // 处理之后的源文件的备份目录
UINT8  g_szCtlFileName[256]  = {0};         // 控制文件全路径名
UINT8  g_szDataStartTime[50] = {0};         // 所有源文件中数据记录的最早开始时间
UINT8  g_szDataEndTime[50]   = {0};         // 所有源文件中数据记录的最晚结束时间
UINT32 g_iRecordsSum         = 0;           // 已处理的记录的总条数


/**********************************************************************
* 功能描述：主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人            修改内容
* -------------------------------------------------------------------
* 20151102        V1.0     Zhou Zhaoxiong        创建
***********************************************************************/
INT32 main()
{
    ReadCtlFile();    // 获取控制文件存放路径、控制文件全路径名及文件内容字段值
	
    ReadSrcFileAndWriteCtlFile();    // 扫描源文件目录, 并写控制文件

    return 0;
}


/**********************************************************************
* 功能描述： 读取控制文件中的开始时间、结束时间和记录条数
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void ReadCtlFile(void)
{
    UINT8 *pszHomePath = NULL; 
    FILE  *fpCtlFile   = NULL;
    UINT8  szBuf[500]  = {0}; 
	
    // 读取控制文件中的开始时间、结束时间和记录条数, 如果是当天程序重启, 则记录条数继续编号
    pszHomePath = getenv("HOME");
    if (pszHomePath == NULL)
    {
        return;
    }
	
    snprintf(g_szCtlFileDir, sizeof(g_szCtlFileDir)-1, "%s/zhouzhaoxiong/zzx/CtlFileCreate/CtlFile", pszHomePath);  // 控制文件存放目录
	
    snprintf(g_szCtlFileName, sizeof(g_szCtlFileName)-1, "%s/CtlFile.txt", g_szCtlFileDir);  // 控制文件全路径名
	
    fpCtlFile = fopen(g_szCtlFileName, "r");
    if (fpCtlFile != NULL)
    {
        fgets(szBuf, sizeof(szBuf), fpCtlFile);
        
        // 获取开始时间g_szDataStartTime
        if (TRUE != GetValueFromStr(1, MML_STR_TYPE, szBuf, g_szDataStartTime, '|', sizeof(g_szDataStartTime)))
        {
            printf("ReadCtlFile: exec GetValueFromStr to get g_szDataStartTime failed!\n");
            return;
        }
 
        // 获取结束时间g_szDataEndTime
        if (TRUE != GetValueFromStr(2, MML_STR_TYPE, szBuf, g_szDataEndTime, '|', sizeof(g_szDataEndTime)))
        {
            printf("ReadCtlFile: exec GetValueFromStr to get g_szDataEndTime failed!\n");
            return;
        }
 
        // 获取记录条数g_iRecordsSum
        if (TRUE != GetValueFromStr(3, MML_INT32_TYPE, szBuf, (UINT8 *)&g_iRecordsSum, '|', sizeof(g_iRecordsSum)))
        {
            printf("ReadCtlFile: exec GetValueFromStr to get g_iRecordsSum failed!\n");
            return;
        }
        
        fclose(fpCtlFile);
        fpCtlFile = NULL;
		
	printf("ReadCtlFile: DataStartTime=%s, DataEndTime=%s, RecordsSum=%d\n", g_szDataStartTime, g_szDataEndTime, g_iRecordsSum);
    }
}


/**********************************************************************
* 功能描述： 扫描源文件目录, 并写控制文件
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void ReadSrcFileAndWriteCtlFile(void)
{
    UINT8 *pszHomePath        = NULL; 
    UINT8  szCommandBuf[500]  = {0}; 
    UINT8  szSrcFile[500]     = {0}; 
	
    DIR            *pDir    = NULL;
    struct dirent  *pDirent = NULL;
	
    pszHomePath = getenv("HOME");
    if (pszHomePath == NULL)
    {
        return;
    }
	
    snprintf(g_szSourceDir,    sizeof(g_szSourceDir)-1,    "%s/zhouzhaoxiong/zzx/CtlFileCreate/SrcFile", pszHomePath);      // 源文件存放目录

    snprintf(g_szSourceBakDir, sizeof(g_szSourceBakDir)-1, "%s/zhouzhaoxiong/zzx/CtlFileCreate/SrcFile_bak", pszHomePath);  // 源文件备份目录
	
    while (1)
    {   
        pDir = opendir(g_szSourceDir);
        if (NULL == pDir)
        {
            printf("ReadSrcFileAndWriteCtlFile: pDir is NULL!\n");
            continue;
        }

        while ((pDirent = readdir(pDir)) != NULL)    // 扫描源目录, 获取文件名
        {
            if (strncmp(pDirent->d_name, "Test_", strlen("Test_")) == 0)      // 如果匹配上了源文件的前缀, 则读取文件内容并写控制文件
            {
                memset(szSrcFile, 0x00, sizeof(szSrcFile));
                snprintf(szSrcFile, sizeof(szSrcFile)-1, "%s/%s", g_szSourceDir, pDirent->d_name, g_szSourceBakDir);
                GetSrcFileContentAndWriteCtlFile(szSrcFile);     // 获取源文件中的内容, 并写控制文件

                // 处理完成之后, 将文件剪切到备份目录中
                memset(szCommandBuf, 0x00, sizeof(szCommandBuf));
                snprintf(szCommandBuf, sizeof(szCommandBuf)-1, "mv %s %s", szSrcFile, g_szSourceBakDir);
                system(szCommandBuf);
                printf("ReadSrcFileAndWriteCtlFile: now, move %s to %s\n", pDirent->d_name, g_szSourceBakDir);
            }
        }
        
        closedir(pDir);
        pDir = NULL;

        Sleep(60 * 1000);    // 每1分钟扫描一次
    }
}


/**********************************************************************
* 功能描述： 获取源文件中的内容, 并写控制文件
* 输入参数： pszSrcFileName-带路径的源文件名
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void GetSrcFileContentAndWriteCtlFile(UINT8 *pszSrcFileName)
{
    FILE  *fp                  = NULL;
    UINT8  szContentLine[1024] = {0};
	
    T_SrcFileContent tSrcFileContent = {0};
	
    if (pszSrcFileName == NULL)
    {
        printf("GetSrcFileContentAndWriteCtlFile: pDir is NULL!\n");
	return;
    }
	
    if ((fp = fopen(pszSrcFileName, "r")) == NULL)  // 只读方式打开
    {
        printf("GetSrcFileContentAndWriteCtlFile: open src file failed!\n");
        return; 
    }
    else
    {
        while (feof(fp) == 0 && ferror(fp) == 0)
        {
            // 每行对应一条源文件记录
            memset(szContentLine, 0x00, sizeof(szContentLine));
            if (fgets(szContentLine, sizeof(szContentLine), fp) == NULL)
            {
                printf("GetSrcFileContentAndWriteCtlFile: exec fgets to get line null.\n");
            }
            else
            {
                printf("GetSrcFileContentAndWriteCtlFile: get content line: %s\n", szContentLine);
            }
			
	    RemoveLineEnd(szContentLine); // 去掉字符串后面的回车换行符

            if (strlen(szContentLine) == 0)   // 如果为空行, 则继续处理下一条
            {
                printf("GetSrcFileContentAndWriteCtlFile: the length of ContentLine is 0, continue.\n"); 
                continue;
            }
			
	    GetSrcFileFieldValue(szContentLine, &tSrcFileContent);   // 获取一条记录中各个字段的值

            memset(szContentLine, 0x00, sizeof(szContentLine));
            GetCtlFileContentAndWrite(&tSrcFileContent, szContentLine); // 组装写入控制文件中的内容
			
	    WriteToCtlFile(szContentLine);    // 将内容写到控制文件中
	}

        fclose(fp);
        fp = NULL;
    }
}


/**********************************************************************
* 功能描述： 组装写入控制文件中的内容
* 输入参数： ptSrcFileContent-源文件中一条记录中各个字段的值
* 输出参数： pszContentBuffer-存放内容的缓存
* 返 回 值： 无
* 其它说明： 控制文件中记录为: DataStartTime|DataEndTime|RecordsSum|
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void GetCtlFileContentAndWrite(T_SrcFileContent *ptSrcFileContent, UINT8 *pszContentBuffer)
{
    UINT8  szContentLine[500] = {0};
    
    if (ptSrcFileContent == NULL || pszContentBuffer == NULL)
    {
        printf("GetCtlFileContentAndWrite: ptSrcFileContent or pszContentBuffer is NULL!\n");
        return;
    }
	
    // 根据值的大小对g_szDataStartTime进行赋值
    if (strlen(g_szDataStartTime) == 0)   // 当天第一条
    {
        strncpy(g_szDataStartTime, ptSrcFileContent->szDataStartTime, strlen(ptSrcFileContent->szDataStartTime));
    }
    else
    {
        if (strncmp(g_szDataStartTime, ptSrcFileContent->szDataStartTime, strlen(ptSrcFileContent->szDataStartTime)) > 0)  // 修改成最小时间
        {
            memset(g_szDataStartTime, 0x00, sizeof(g_szDataStartTime));

            strncpy(g_szDataStartTime, ptSrcFileContent->szDataStartTime, strlen(ptSrcFileContent->szDataStartTime));
        }
    }

    // 根据值的大小对g_szDataEndTime进行赋值
    if (strlen(g_szDataEndTime) == 0)   // 当天第一条
    {
        strncpy(g_szDataEndTime, ptSrcFileContent->szDataEndTime, strlen(ptSrcFileContent->szDataEndTime));
    }
    else
    {
        if (strncmp(g_szDataEndTime, ptSrcFileContent->szDataEndTime, strlen(ptSrcFileContent->szDataEndTime)) < 0)  // 修改成最大时间
        {
            memset(g_szDataEndTime, 0x00, sizeof(g_szDataEndTime));

            strncpy(g_szDataEndTime, ptSrcFileContent->szDataEndTime, strlen(ptSrcFileContent->szDataEndTime));
        }
    }

    // 记录总条数加1
    g_iRecordsSum = g_iRecordsSum + 1;     // 当天所有记录的总条数加1

    // 打印三个字段的内容
    printf("GetCtlFileContentAndWrite: DataStartTime is %s, DataEndTime is %s, RecordsSum is %d\n", g_szDataStartTime, g_szDataEndTime, g_iRecordsSum);

    // 组装写到控制文件中的消息内容
    snprintf(szContentLine, sizeof(szContentLine)-1, "%s|%s|%d|", g_szDataStartTime, g_szDataEndTime, g_iRecordsSum);

    printf("GetCtlFileContentAndWrite: ContentLine is %s\n", szContentLine);

    strncpy(pszContentBuffer, szContentLine, strlen(szContentLine));
}


/**********************************************************************
* 功能描述： 获取源文件中的各个字段的值
* 输入参数： pszContentLine-一条记录
* 输出参数： ptSrcFileContent-源文件中一条记录中各个字段的值
* 返 回 值： 无
* 其它说明： 源文件中每条记录的格式为: SrcNumber|DstNumber|DataStartTime|DataEndTime|
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void GetSrcFileFieldValue(UINT8 *pszContentLine, T_SrcFileContent *ptSrcFileContent)
{
    if (pszContentLine == NULL || ptSrcFileContent == NULL)
    {
        printf("GetSrcFileFieldValue: ContentLine or SrcFileContent is NULL!\n");
	return;
    }
        
    // 获取源号码
    if (TRUE != GetValueFromStr(1, MML_STR_TYPE, pszContentLine, ptSrcFileContent->szSrcNumber, '|', sizeof(ptSrcFileContent->szSrcNumber)))
    {
        printf("GetSrcFileFieldValue: exec GetValueFromStr to get szSrcNumber failed!\n");
        return;
    }
 
    // 获取目的号码
    if (TRUE != GetValueFromStr(2, MML_STR_TYPE, pszContentLine, ptSrcFileContent->szDstNumber, '|', sizeof(ptSrcFileContent->szDstNumber)))
    {
        printf("GetSrcFileFieldValue: exec GetValueFromStr to get szDstNumber failed!\n");
        return;
    }
	
    // 获取开始时间
    if (TRUE != GetValueFromStr(3, MML_STR_TYPE, pszContentLine, ptSrcFileContent->szDataStartTime, '|', sizeof(ptSrcFileContent->szDataStartTime)))
    {
        printf("GetSrcFileFieldValue: exec GetValueFromStr to get szDataStartTime failed!\n");
        return;
    }
	
    // 获取结束时间
    if (TRUE != GetValueFromStr(4, MML_STR_TYPE, pszContentLine, ptSrcFileContent->szDataEndTime, '|', sizeof(ptSrcFileContent->szDataEndTime)))
    {
        printf("GetSrcFileFieldValue: exec GetValueFromStr to get szDataEndTime failed!\n");
        return;
    }
		
    printf("GetSrcFileFieldValue: SrcNumber=%s, DstNumber=%s, DataStartTime=%s, DataEndTime=%s\n", ptSrcFileContent->szSrcNumber, ptSrcFileContent->szDstNumber, 
	                                                                            ptSrcFileContent->szDataStartTime, ptSrcFileContent->szDataEndTime);
	
	
}

 
/**********************************************************************
* 功能描述： 程序休眠
* 输入参数： iCountMs-休眠时间(单位:ms)
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void Sleep(UINT32 iCountMs)
{
    struct timeval t_timeout = {0};
 
    if (iCountMs < 1000)
    {
        t_timeout.tv_sec = 0;
        t_timeout.tv_usec = iCountMs * 1000;
    }
    else
    {
        t_timeout.tv_sec = iCountMs / 1000;
        t_timeout.tv_usec = (iCountMs % 1000) * 1000;
    }
    select(0, NULL, NULL, NULL, &t_timeout);   // 调用select函数阻塞程序
}


/**********************************************************************
*功能描述：获取字符串中某一个字段的值
*输入参数：iSerialNum-字段编号(为正整数)
           iContentType-需要获取的内容的类型
           pSourceStr-源字符串
           pDstStr-目的字符串(提取的值的存放位置)
           cIsolater-源字符串中字段的分隔符
           iDstStrSize-目的字符串的长度
*输出参数：无
*返 回 值：TRUE-成功  FALSE-失败
*其它说明：无
*修改日期        版本号            修改人         修改内容
* --------------------------------------------------------------
* 20151102        V1.0         Zhou Zhaoxiong       创建
***********************************************************************/
BOOL GetValueFromStr(UINT16 iSerialNum, UINT8 iContentType, UINT8 *pSourceStr, UINT8 *pDstStr, UINT8 cIsolater, UINT32 iDstStrSize)
{
    UINT8  *pStrBegin                 = NULL;
    UINT8  *pStrEnd                   = NULL;
    UINT8   szRetBuf[MAX_RET_BUF_LEN] = {0};     // 截取出的字符串放入该数组中
    UINT8  *pUINT8                    = NULL;
    UINT16 *pUINT16                   = NULL;
    UINT32 *pUINT32                   = NULL;
    UINT32  iFieldLen                 = 0;      // 用于表示每个字段的实际长度
 
    if (pSourceStr == NULL)                     // 对输入指针的异常情况进行判断
    {
        return FALSE;
    }
    //字段首
    pStrBegin = pSourceStr;
    while (--iSerialNum != 0)
    {
        pStrBegin = strchr(pStrBegin, cIsolater);
        if (pStrBegin == NULL)
        {
            return FALSE;
        }
        pStrBegin ++;
    }
 
    //字段尾
    pStrEnd = strchr(pStrBegin, cIsolater);
    if (pStrEnd == NULL)
    {
        return FALSE;
    }
 
    iFieldLen = (UINT16)(pStrEnd - pStrBegin);
    if(iFieldLen >= MAX_RET_BUF_LEN) //进行异常保护, 防止每个字段的值过长
    {
        iFieldLen = MAX_RET_BUF_LEN - 1;
    }
 
    memcpy(szRetBuf, pStrBegin, iFieldLen);
 
    //将需要的字段值放到pDstStr中去
    switch (iContentType)
    {
        case MML_STR_TYPE:                        //字符串类型
        {
            strncpy(pDstStr, szRetBuf, iDstStrSize);
            break;
        }
 
        case MML_INT8_TYPE:                       //字符类型
        {
            pUINT8   = (UINT8 *)pDstStr;
            *pDstStr = (UINT8)atoi(szRetBuf);
            break;
        }
 
        case MML_INT16_TYPE:                      // short int类型
        {
            pUINT16  = (UINT16 *)pDstStr;
            *pUINT16 = (UINT16)atoi(szRetBuf);
            break;
        }
 
        case MML_INT32_TYPE:                      // int类型
        {
            pUINT32  = (UINT32 *)pDstStr;
            *pUINT32 = (UINT32)atoi(szRetBuf);
            break;
        }
 
        default:                                  // 一定要有default分支
        {
            return FALSE;
        }
    }
 
    return TRUE;
}


/**********************************************************************
* 功能描述： 去掉字符串后面的回车换行符
* 输入参数： pszStr-输入的字符串
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期          版本号       修改人              修改内容
* ------------------------------------------------------------------
* 20151102          V1.0     Zhou Zhaoxiong           创建
********************************************************************/
void RemoveLineEnd(UINT8 *pszStr)
{
    UINT32  iStrLen = 0;
	
    if (pszStr == NULL)
    {
        printf("RemoveLineEnd: pszStr is NULL!\n");
	return;
    }

    iStrLen = strlen(pszStr);
    while (iStrLen > 0)
    {
        if (pszStr[iStrLen-1] == '\n' || pszStr[iStrLen-1] == '\r')
	{
            pszStr[iStrLen-1] = '\0';
	}
        else
	{
            break;
	}

        iStrLen --;
    }

    return;
}


/**********************************************************************
 * 功能描述： 把内容写到控制文件中
 * 输入参数： pszContentLine-一条文件记录
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 无
 * 修改日期        版本号     修改人            修改内容
 * ------------------------------------------------------
 * 20151103        V1.0     Zhou Zhaoxiong       创建
 ***********************************************************************/
void WriteToCtlFile(UINT8 *pszContentLine)
{
    FILE  *fpCtlFile = NULL;

    if (pszContentLine == NULL)
    {
        printf("WriteToCtlFile: pszContentLine is NULL.\n");
        return;
    }
    
    fpCtlFile = fopen(g_szCtlFileName, "w");
    if (fpCtlFile != NULL)
    {
        fputs(pszContentLine, fpCtlFile);
        fclose(fpCtlFile);
        fpCtlFile = NULL;
    
        printf("WriteToCtlFile: write ctl file successfully! file=%s, content=%s\n", g_szCtlFileName, pszContentLine);
    }
    else
    {
        printf("WriteToCtlFile: write ctl file failed! file=%s, content=%s\n", g_szCtlFileName, pszContentLine);
    }
}

