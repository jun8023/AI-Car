#include "all.h"
#include "Catch.h"

uint8   date[X_MAX+6][Y_MAX+15],center[dot_num][2];
float input=0,input_error=0,output=0,p=10;
uint8 OK_FLAG=0;
uint8 root1=0,root2=0,change_left=0,change_right=0;
uint8 line_left=0,line_right=0,left_start,right_start,left_end,right_end;
uint8 left=0,right=0,left_flag=0,right_flag=0,left_last,right_last;
uint32 dif=0;
//中值滤波参数
//边线扫描算法
uint8 left_line[200][2],right_line[200][2],left_dot=0,right_dot=0,left_10[2],right_10[2],left_10_i,left_10_j,right_10_i,right_10_j,ones_ok=0,ten_hang,ten_lie,ten_left_ok=0,ten_right_ok=0;
int8 dot_sub_left=0,dot_sub_right=0;
uint8 scan_mode_left=0,scan_mode_right=0,type=0;//表明赛道类型;//0表示行扫描，1表示列扫描
uint8 left_white_time = 0, left_dark_time=0,left_l_dir = 0, right_white_time=0, right_dark_time=0, right_h_dir = 0, right_l_dir = 0;//设定转换时扫描方向  小于0表示按递减  1表示按递增
uint8 error_times_left=0,error_times_right=0,left_i=0,left_j=0,right_i=0,right_j=0,hang_dir=0,lie_dir=0,left_end_flag=1,right_end_flag=1;//0表示搜完了
uint32 temp;
float test=0;       
//直角参数
uint8 angle_ok = 0, angle_hang, angle_lie,angle_left_ok = 0, angle_right_ok = 0,left_angle_i,left_angle_j,right_angle_i,right_angle_j,left_angle[2],right_angle[2];
//单线参数
uint8 test_single_line = 0, single_left[2], single_right[2], single_left_ok, single_right_ok,start_single=0;//单线检测


void CAM_get()
 {
     uint8 i,j;uint8 successed=1;
     uint8 temp1,temp2,temp3;
     uint8 threshold=white-dif*2/3;
#if    0        //二值化
                   dif=white-dark;
                   for(i=0;i<X_MAX;i++)
                       for(j=0;j<Y_MAX;j++)
                        {
                            if(date[i][j]>=white-dif*2/3)
                                date[i][j]=254;
                            if(date[i][j]<=dark+dif*2/3)
                                date[i][j]=1;
                        }
#endif
#if     1           //边缘扫描算法		//参数初始化
	scan_mode_left = scan_mode_right = 0;//0表示行扫描，1表示列扫描
	left_dot = 0; right_dot = 0;		//采到的左右点数
	left_l_dir = 0;		//左右前进方向
	right_l_dir = 0;		//左右前进方向
	left_flag = 0, right_flag = 0;	//是否采集到点标志位
	left_i = right_i = 0;
	left_j = right_j = 0;
	type = 0;//表明赛道类型
	left_end_flag = 1, right_end_flag = 1;//扫完了置0			 
	start_single=0;
	for (i = start; i <= end; i++) //扫判断是否有中线，如果有的话就根据中线跑
	{
		for (j = 10; j < Y_MAX - 10; j++)
		{
			if ((date[i][j + 1] <threshold) && (date[i][j + 2]<threshold)&& (date[i][j ] >threshold))
			{
				right = j;
				left_flag = 1;
				break;
			}
			left_flag = 0;
			right_flag=0;
		}

		if (left_flag)
		{
			for (j; j < Y_MAX - 10; j++)
			{
				if ((date[i][j + 1] >threshold) && (date[i][j + 2] >threshold)&&(date[i][j]<threshold))
				{
					left = j;
					right_flag = 1;
					break;
				}
				right_flag = 0;
			}
		}
		if (right_flag&&(left<right+10))
		{
			start_single ++;
			right_line[right_dot][0] = i; right_line[right_dot][1] = right; right_dot++;
			left_line[left_dot][0] = i; left_line[left_dot][1] = left; left_dot++;
		}
		else
		{
			//未处理
		}

	}
	if (start_single<=(end/2))//如果采样单线较少说明并不是单线
	{
		left_dot=right_dot=0;
		for (left_i = right_i = start; (!(((left_flag) && (right_flag)) && (left_i >= end) && (right_i >= end)));)   //先扫前几行，确定有效边界
		{
			left = 0; right = Y_MAX;
			if ((!left_flag) || (left_i <= end))
			{
				for (j = Y_MAX / 2; j>3; j--)
				{
					if ((date[left_i][j - 2] <threshold) && (date[left_i][j - 1]<threshold))
					{
						left = j;
						left_flag = 1;
						break;
					}
					left_flag = 0;
				}
				left_line[left_dot][0] = left_i; left_line[left_dot][1] = left; left_dot++; left_i++;
				if((left_dot<3)||(!((left_line[left_dot-1][1]+scale>left_line[left_dot-3][1])&&(left_line[left_dot-3][1]+scale>left_line[left_dot-1][1])))) left_flag=0;
			}
			if ((!right_flag) || (right_i <= end))
			{
				for (j = Y_MAX / 2; j<=Y_MAX - 3; j++)
				{
					if ((date[right_i][j + 1] <threshold) && (date[right_i][j + 2] <threshold))
					{
						right = j;
						right_flag = 1;
						break;
					}
					right_flag = 0;
				}
				right_line[right_dot][0] = right_i; right_line[right_dot][1] = right; right_dot++; right_i++;
				if((right_dot<3)||(!((right_line[right_dot-1][1]+scale>right_line[right_dot-3][1])&&(right_line[right_dot-3][1]+scale>right_line[right_dot-1][1])))) right_flag=0;
			}
			if ((left_i > 100) || (right_i > 100))  break;//如果有一边超过100行，说明出问题了，退出
		}//先搜索完前至少end行确定有效边界
	}
	//左右连线
	for (left_i=left_line[left_dot-1][0], right_i=right_line[right_dot-1][0], left_j = left_line[left_dot - 1][1], right_j = right_line[right_dot - 1][1];successed&&( left_end_flag || right_end_flag);)
	{
		//非行扫描确定赛道类型
	
		if (!(scan_mode_left | scan_mode_right))//两个都是行扫描方式
		{
			if (!change_left)//左边找不到，停止
			{
                left_flag=0;
				root1 = left_line[left_dot - 1][1];
				//	root1=left_line[left_dot-1][1]-left_line[left_dot-2][1]+left_line[left_dot-1][1];
				if (date[left_i][root1] >threshold)//如果上面那个为白点，往左搜
				{
					if (root1<scale) left_end = 3; else left_end = root1 - scale;
					for (left_j = root1; left_j >= left_end; left_j--)
					{
						if ((date[left_i][left_j - 2] <threshold) && (date[left_i][left_j - 1] <threshold))
						{
							left = left_j;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left_i; left_line[left_dot][1] = left; left_dot++;
							break;
						}
						left_flag = 0;
					}
					if (left_flag == 0) error_times_left++;
					if (error_times_left >= 1) change_left = 1;
				}
				else
				{
					if (root1 + scale>Y_MAX - 3) left_end = Y_MAX - 3; else left_end = root1 + scale;
					for (left_j = root1; left_j<left_end; left_j++)
					{
						if ((date[left_i][left_j + 2] >threshold) && (date[left_i][left_j + 1] >threshold))
						{
							left = left_j + 1;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left_i; left_line[left_dot][1] = left; left_dot++;
							break;
						}
						left_flag = 0;
					}
					if (left_flag == 0) error_times_left++;
					if (error_times_left >= 1) change_left = 1;
				}
				left_i++;
			}
			if (!change_right)//右边找 不到，停止
			{
				root2 = right_line[right_dot - 1][1];right_flag=0;
				//	root2=right_line[right_dot-1][1]-right_line[right_dot-2][1]+right_line[right_dot-1][1];
				if (date[right_i][root2] >threshold)//如果上面那个为白点，往右搜
				{
					if (root2 + scale>Y_MAX - 3) right_end = Y_MAX - 3; else right_end = root2 + scale;
					for (right_j = root2; right_j<right_end; right_j++)
					{
						if ((date[right_i][right_j + 2] <threshold) && (date[right_i][right_j + 1] <threshold))
						{
							right = right_j;
							error_times_right = 0;
							right_flag = 1;//表示采集到右边缘
							right_line[right_dot][0] = right_i; right_line[right_dot][1] = right; right_dot++;
							break;
						}
						right_flag = 0;
					}
					if (right_flag == 0) error_times_right++; if (error_times_right >= 1) change_right = 1;
				}
				else
				{
					if (root2<scale) right_end = 3; else right_end = root2 - scale;
					for (right_j = root2; right_j>right_end; right_j--)
					{
						if ((date[right_i][right_j - 2] >threshold) && (date[right_i][right_j - 1] >threshold))
						{
							right = right_j - 1;
							error_times_right = 0;
							right_flag = 1;//表示采集到左边缘
							right_line[right_dot][0] = right_i; right_line[right_dot][1] = right; right_dot++;
							break;
						}
						right_flag = 0;
					}
					if (right_flag == 0) error_times_right++; if (error_times_right >= 1) change_right = 1;
				}
				right_i++;
			}
		}
			if (change_right&change_left)//确定扫描方式
		{
			left_white_time = left_dark_time = 0;
			right_white_time = right_dark_time = 0;
			change_right = change_left = 0;
			for (i = 1; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
					if (date[left_line[left_dot - 1][0] + i][left_line[left_dot - 1][1] + j - 1] >threshold)
						left_white_time++;
					else left_dark_time++;
			}
			if (left_white_time > left_dark_time) left_l_dir = 0; else left_l_dir = 1;
			for (i = 1; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
					if (date[right_line[right_dot - 1][0] + i][right_line[right_dot - 1][1] + j - 1] >threshold)
						right_white_time++;
					else right_dark_time++;
			}
			if (right_white_time > right_dark_time) right_l_dir = 1; else right_l_dir = 0;

			if (left_l_dir&right_l_dir) type = 1;//遇到直角或弯道，向增大偏
			else if (!(left_l_dir || right_l_dir)) type = 2;//遇到直角或弯道，向减小偏
			else if ((!left_l_dir) && (right_l_dir))//表明可能遇到十字
			{
				single_left_ok = single_right_ok = 0;
				for (test_single_line = left_line[left_dot - 1][1]; test_single_line <= right_line[right_dot - 1][1]; test_single_line++)
				{
					if ((!single_left_ok) && (date[left_line[left_dot - 1][0] + 3][test_single_line]<threshold) && (date[left_line[left_dot - 1][0] + 3][test_single_line + 1] <threshold))
					{
						single_left[0] = left_line[left_dot - 1][0] + 3;//行数确定
						single_left[1] = test_single_line;
						single_left_ok = 1;//表明搜索到单线
					}
					if ((single_left_ok) && (date[left_line[left_dot - 1][0] + 3][test_single_line] >threshold) && (date[left_line[left_dot - 1][0] + 3][test_single_line + 1] >threshold))
					{
						single_right[0] = left_line[left_dot - 1][0] + 5;//行数确定
						single_right[1] = test_single_line;
						single_right_ok = 1;//表明搜索到单线
						break;
					}
				}
				if (single_left_ok&single_right_ok)
				{//单线左右转换，便于扫描
					left_line[left_dot][0] = single_right[0]; left_line[left_dot][1] = single_right[1];  left_dot++;
					right_line[right_dot][0] = single_left[0]; right_line[right_dot][1] = single_left[1]; right_dot++;
					type = 5;//表明遇到了单线
				}
				else type = 3;//、表明是十字   
			}
			else type = 4;//表明遇到直角
			scan_mode_left = scan_mode_right = 1;//开始竖着扫
			left_j = left_line[left_dot - 1][1];
			right_j = right_line[right_dot - 1][1];
		}
		switch (type)		//说明是其中之一，执行完break;
		{
        case 0:successed=0;break;
		case 1:
		{
			if (left_end_flag)	//左边没找完，竖着找
			{
				root1 = left_line[left_dot - 1][0];
				//	root1=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
				if (date[root1][left_j] >threshold)//如果上面那个为白点，往上搜
				{
					if (root1<scale) left_end = 3; else left_end = root1 + scale;
					for (left_i = root1; left_i<left_end; left_i++)
					{
						if ((date[left_i + 2][left_j] <threshold) && (date[left_i + 1][left_j] <threshold))
						{
							left = left_i;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left; left_line[left_dot][1] = left_j; left_dot++;
							break;
						}
						left_flag = 0;
					}
				}
				else
				{
					if (root1 + scale>Y_MAX - 3) left_end = Y_MAX - 3; else left_end = root1 - scale;
					for (left_i = root1; left_i>left_end; left_i--)
					{
						if ((date[left_i - 2][left_j] >threshold) && (date[left_i - 1][left_j] >threshold))
						{
							left = left_i - 1;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left; left_line[left_dot][1] = left_j; left_dot++;
							break;
						}
						left_flag = 0;
					}
				}
				left_j++;
			}
			if (right_end_flag)//右边没找完，竖着找
			{
				root2 = right_line[right_dot - 1][0];
				//	root2=right_line[right_dot-1][0]-right_line[right_dot-2][0]+right_line[right_dot-1][0];
				if (date[root2][right_j] <threshold)//如果上面那个为黑点，往上搜
				{
					if (root2<scale) right_end = 3; else right_end = root2 + scale;
					for (right_i = root2; right_i<right_end; right_i++)
					{
						if ((date[right_i + 2][right_j] >threshold) && (date[right_i + 1][right_j] >threshold))
						{
							right = right_i + 1;
							error_times_right = 0;
							right_flag = 1;//表示采集到左边缘
							right_line[right_dot][0] = right; right_line[right_dot][1] = right_j; right_dot++;
							break;
						}
						right_flag = 0;
					}
				}
				else
				{
					if (root2 + scale>Y_MAX - 3) right_end = Y_MAX - 3; else right_end = root2 - scale;
					for (right_i = root2; right_i>right_end; right_i--)
					{
						if ((date[right_i - 2][right_j] <threshold) && (date[right_i - 1][right_j]<threshold))
						{
							right = right_i;
							error_times_right = 0;
							right_flag = 1;//表示采集到左边缘
							right_line[right_dot][0] = right; right_line[right_dot][1] = right_j; right_dot++;
							break;
						}
						right_flag = 0;
					}
				}
				right_j++;
			}
		}break;
		case 2:
		{
			if (left_end_flag)	//左边没找完，竖着找
			{
				root1 = left_line[left_dot - 1][0];
				//	root1=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
				if (date[root1][left_j] >threshold)//如果上面那个为白点，往下搜
				{
					if (root1<scale) left_end = 3; else left_end = root1 - scale;
					for (left_i = root1; left_i>left_end; left_i--)
					{
						if ((date[left_i - 2][left_j]<threshold) && (date[left_i - 1][left_j]<threshold))
						{
							left = left_i;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left; left_line[left_dot][1] = left_j; left_dot++;
							break;
						}
						left_flag = 0;
					}
				}
				else
				{
					if (root1 + scale>Y_MAX - 3) left_end = Y_MAX - 3; else left_end = root1 + scale;
					for (left_i = root1; left_i<left_end; left_i++)
					{
						if ((date[left_i + 2][left_j] >threshold) && (date[left_i + 1][left_j] >threshold))
						{
							left = left_i + 1;
							error_times_left = 0;
							left_flag = 1;//表示采集到左边缘
							left_line[left_dot][0] = left; left_line[left_dot][1] = left_j; left_dot++;
							break;
						}
						left_flag = 0;
					}
				}
				left_j--;
			}
			if (right_end_flag)//右边没找完，竖着找
			{
				root2 = right_line[right_dot - 1][0];
				//	root2=right_line[right_dot-1][0]-right_line[right_dot-2][0]+right_line[right_dot-1][0];
				if (date[root2][right_j] <threshold)//如果上面那个为黑点，往下搜
				{
					if (root2<scale) right_end = 3; else right_end = root2 - scale;
					for (right_i = root2; right_i>right_end; right_i--)
					{
						if ((date[right_i - 2][right_j] >threshold) && (date[right_i - 1][right_j]>threshold))
						{
							right = right_i - 1;
							error_times_right = 0;
							right_flag = 1;//表示采集到左边缘
							right_line[right_dot][0] = right; right_line[right_dot][1] = right_j; right_dot++;
							break;
						}
						right_flag = 0;
					}
				}
				else
				{
					if (root2 + scale>Y_MAX - 3) right_end = Y_MAX - 3; else right_end = root2 + scale;
					for (right_i = root2; right_i<right_end; right_i++)
					{
						if ((date[right_i + 2][right_j] <threshold) && (date[right_i + 1][right_j] <threshold))
						{
							right = right_i;
							error_times_right = 0;
							right_flag = 1;//表示采集到左边缘
							right_line[right_dot][0] = right; right_line[right_dot][1] = right_j; right_dot++;
							break;
						}
						right_flag = 0;
					}
				}
				right_j--;
			}
		}break;
		case 3: {
			ones_ok = 0; ten_left_ok = ten_right_ok = 0;
			ten_hang = (left_line[left_dot - 1][0] + right_line[right_dot - 1][0]) / 2 + 15;//往后先扫10行
			ten_lie = (left_line[left_dot - 1][1] + right_line[right_dot - 1][1]) / 2;
			while (!ones_ok)//判断补线是否成功
			{
				for (ten_hang; (!(ten_left_ok&ten_right_ok)) && (ten_hang<X_MAX - 3); ten_hang += 5)
				{
					for (left_10_j = ten_lie; (!ten_left_ok) && (left_10_j>3); left_10_j--)
					{
						if ((date[ten_hang][left_10_j - 2] <threshold) && (date[ten_hang][left_10_j - 1] <threshold))
						{

							ten_left_ok = 1;
							left_10[1] = left_10_j;
							left_10[0] = ten_hang;
							break;
						}
						ten_left_ok = 0;
					}

					for (right_10_j = ten_lie; (right_10_j<Y_MAX - 3) && (!ten_right_ok); right_10_j++)
					{
						if ((date[ten_hang][right_10_j + 2] <threshold) && (date[ten_hang][right_10_j + 1]<threshold))
						{

							ten_right_ok = 1;
							right_10[1] = right_10_j;
							right_10[0] = ten_hang;
							break;
						}
						ten_right_ok = 0;
					}
				}//十字没有补线的必要，跑的时候以下代码可以忽略
				ones_ok = 1;//表明一次以过
							/*dot_sub_left=(left_10[0]-left_line[left_dot-3][0])/(left_10[1]-left_line[left_dot-3][1]);
							for(left_i;left_i<left_10[0];left_i++)
							{
							left_line[left_dot][0]=i;
							if((left_10[0]-i)%dot_sub_left==0)
							left_line[left_dot][1]=left_line[left_dot-1][1]+1;
							else left_line[left_dot][1]=left_line[left_dot-1][1];
							left_dot++;
							}

							dot_sub_right=(right_10[0]-right_line[right_dot-3][0])/(right_10[1]-right_line[right_dot-3][1]);
							for(right_i;right_i<right_10[0];right_i++)
							{
							right_line[right_dot][0]=i;
							if((right_10[0]-i)%dot_sub_right==0)
							right_line[right_dot][1]=right_line[right_dot-1][1]+1;
							else right_line[right_dot][1]=right_line[right_dot-1][1];
							right_dot++;
							}*/

							//如果两个都找到~从直线重新开始
				if (ten_left_ok&ten_right_ok)//搜到重新定位扫描然后和直线弯道一样处理
				{
					type = 0; scan_mode_left = scan_mode_right = 0;
					left_line[left_dot][0] = left_i = left_10[0]; left_line[left_dot][1] = left_j = left_10[1]; left_dot++;
					right_line[left_dot][0] = right_i = right_10[0]; right_line[right_dot][1] = right_j = right_10[1]; right_dot++;
				}
				else
				{
					right_i = left_i = (left_line[left_dot - 1][0] + right_line[right_dot - 1][0]) / 2;//往后先扫10行
					right_j = left_j = (left_line[left_dot - 1][1] + right_line[right_dot - 1][1]) / 2;
					for (left_i; left_i <= X_MAX - 2; left_i++)
					{
						left_line[left_dot][0] = left_i; left_line[left_dot][1] = left_j; left_dot++;
					}
					for (right_i; right_i <= X_MAX - 2; right_i++)
					{
						right_line[left_dot][0] = right_i; right_line[right_dot][1] = right_j; right_dot++;
					}
				}
			}
		}break;
		case 4: {
			ones_ok = 0; angle_left_ok = angle_right_ok = 0;
			angle_hang = (left_line[left_dot - 1][0] + right_line[right_dot - 1][0]) / 2 + 10;//往后先扫10行
			angle_lie = (left_line[left_dot - 1][1] + right_line[right_dot - 1][1]) / 2;
			while (!ones_ok)//判断补线是否成功
			{
				for (angle_hang; angle_hang < Y_MAX - 3; angle_hang += 5)
				{
					if ((date[angle_hang][angle_lie] >threshold) && (date[angle_hang][angle_lie + 1]>threshold))//表明搜到黑线之后的白线
					{
						angle_ok = 1;
						break;
					}
					angle_ok = 0;
				}
				ones_ok = 1;//表明搜索过一次，退出
				if (angle_ok)	//如果搜索到白线，找边界退出到直道模式
				{
					for (left_angle_j = angle_lie; (!angle_left_ok) && (left_angle_j>3); left_angle_j -= 2)
					{
						if ((date[angle_hang][left_angle_j - 2] <threshold) && (date[angle_hang][left_angle_j - 1] <threshold))
						{
							angle_left_ok = 1;
							left_angle[1] = left_angle_j;
							left_angle[0] = angle_hang;
							break;
						}
						angle_left_ok = 0;
					}

					for (right_angle_j = angle_lie; (right_angle_j<Y_MAX - 3) && (!angle_right_ok); right_angle_j++)
					{
						if ((date[angle_hang][right_angle_j + 2] <threshold) && (date[angle_hang][right_angle_j + 1]<threshold))
						{
							angle_left_ok = 1;
							right_angle[1] = right_angle_j;
							right_angle[0] = angle_hang;
							break;
						}
						angle_right_ok = 0;
					}
					type = 0; scan_mode_left = scan_mode_right = 0;	//准备直道搜索
					left_line[left_dot][0] = left_i = left_angle[0]; left_line[left_dot][1] = left_j = left_10_j = left_angle[1]; left_dot++;
					right_line[left_dot][0] = right_i = right_angle[0]; right_line[right_dot][1] = right_j = right_angle[1]; right_dot++;
				}
				else         //否则直接补线
				{
					for (left_i; left_i <= X_MAX - 2; left_i++)
					{
						left_line[left_dot][0] = left_i; left_line[left_dot][1] = left_j; left_dot++;
					}
					for (right_i; right_i <= X_MAX - 2; right_i++)
					{
						right_line[right_dot][0] = right_i; right_line[right_dot][1] = right_j; right_dot++;
					}
				}
			}
		}  break;
		case 5:
		{
			type = 0; scan_mode_left = scan_mode_right = 0;	//准备直道搜索，如果搜到底，会按十字处理;
			left_i = left_line[left_dot - 1][0]; left_j = left_line[left_dot - 1][1];
			right_i = right_line[right_dot - 1][0]; right_j = right_line[right_dot - 1][1];
		}break;
		}
		left_end_flag = ((left_i>3) && (left_i<X_MAX - 3) && (left_j>3) && (left_j<Y_MAX - 3));//到了边界，置0
		right_end_flag = ((right_i>3) && (right_i<X_MAX - 3) && (right_j>3) && (right_j<Y_MAX - 3));
	}

#endif                            
    
    
                for(i=0;i<dot_num;i++)
                     {
                         center[i][0]=(left_line[left_dot/dot_num*i][0]+right_line[right_dot/dot_num*i][0])/2;
                         center[i][1]=(left_line[left_dot/dot_num*i][1]+right_line[right_dot/dot_num*i][1])/2;
                     }
                    for(i=0;i<dot_num-2;i++)
                     {
                         if(center[i][1]>center[i+1][1])
                          {
                              temp1=center[i][1];temp2=center[i+1][1];
                          }
                         else 
                          {
                              temp1=center[i+1][1];temp2=center[i][1];
                          }
                         if(center[i+2][1]>temp1) temp3=temp1;
                         else if (center[i+2][1]<temp2) temp3=temp2;
                         else temp3=center[i+2][1];
                         center[i][1]=temp3;
                     }
 }