#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#define X_MAX	130
#define Y_MAX	190
//定义中线起始和终止位置
#define start   0
#define end     15
//确定跟随扫描点个数 
#define scale   4
#define red_line 255*65536 
#define blue_line 255
#define green_line 255*256
typedef unsigned char                       uint8;
typedef unsigned short int                  uint16;
#define uint32  unsigned int               //无符号位数
typedef unsigned long int                   uint64;

typedef volatile    uint8                   vuint8;
typedef volatile    uint16                  vuint16;
typedef volatile    uint32                  vuint32;    //不优化无符号位数

typedef char                                int8;
typedef short int                           int16;
typedef int                                 int32;      //有符号位数
typedef long int                            int64;
__declspec( dllexport ) void change(uint32 date[X_MAX][Y_MAX], unsigned int hang, unsigned int lie,unsigned char white,unsigned char dark);
#endif