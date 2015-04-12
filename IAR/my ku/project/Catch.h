#ifndef __CATCH_H__
#define __CATCH_H__

#define  X_MAX     130
#define  Y_MAX    190
//定义采样点数
#define dot_num 20
//定义中线起始和终止位置
#define start   3
#define end     10
#define  white 130
#define dark    100
extern  uint8   date[X_MAX+5][Y_MAX+15],center[dot_num][2];
//确定跟随扫描点个数
#define scale   5

extern float input,input_error,output,p;

























#endif