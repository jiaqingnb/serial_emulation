/*
 * CM_Assert.h
 *
 *  Created on: 2021年8月19日
 *      Author: GaoHan
 */

#ifndef SRC_MSCP_COMMON_CM_ASSERT_H
#define SRC_MSCP_COMMON_CM_ASSERT_H

#include <QtDebug>

#define QUANFUZADU  1
#ifdef  QUANFUZADU
#define CM_ASSERT(value,_file,_func,_line) \
do{\
    if(value)\
    {\
        qDebug()<<"ASSERT ERR!file =%s,func= %s,line= %d\n"<<_file<<_func<<_line\
    }\
}while(0)
#else
#define CM_ASSERT(value,_file,_func,_line)
#endif




#endif /* SRC_MSCP_COMMON_CM_ASSERT_H_ */
