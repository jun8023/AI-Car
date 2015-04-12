#include "function.h"
void delay(uint32 n)
{
	uint8 i,j;
	for(i=0;i<n;i++)
		for(j=0;j<100;j++);
}

__declspec( dllexport ) void change(uint32 date[X_MAX][Y_MAX], unsigned int hang, unsigned int lie,unsigned char white,unsigned char dark)
{
uint8  i=0,j=0,k=0,t=0;
uint8 OK_FLAG=0;
uint8 root1=0,root2=0,sensor[8]={0};
float sensor_driver=0;
uint8 flag_photo=0,read_date=0,temp_l=0,temp_r=0,time=0,ave_now=0;
uint8 center0,line_left=0,line_right=0,left_start,right_start,left_end,right_end;
uint8 left=0,right=0,center[X_MAX],left_flag=0,right_flag=0,left_last,right_last;
uint32 dif=0;
//边线扫描算法
uint8 left_line[500][2],right_line[500][2];uint16 left_dot=0,right_dot=0,left_10[2],left_10_i,left_10_j,ones_ok=0;
int8 dot_sub=0;
uint8 scan_mode=0;//0表示行扫描，1表示列扫描
uint8 hang_dir=0,lie_dir=0;//设定转换时扫描方向  小于0表示按递减  大于0表示按递增
uint8 error_times=0;
uint32 temp;
float test=0;

/*for(i=0;i<X_MAX;i++)
{
	for(j=0;j<Y_MAX;j++)
	{
		if(date[i][j]=='\0')
		{
			OK_FLAG=1;
			break;
		}
	}
	if(OK_FLAG) break;
}*/
//if(OK_FLAG==0){
#if			1
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
					  //
#if     1           //边缘扫描算法
                   scan_mode=0;left_dot=0;right_dot=0;
                   scan_mode=0;//0表示行扫描，1表示列扫描
                    hang_dir=0;lie_dir=0;
					left_flag=0,right_flag=0;
                   for(i=start;!(((left_flag==1)&&(right_flag==1))&&(i>=end));i++)   //先扫前几行
                    {
                        left=0;right=Y_MAX;
                        for(j=Y_MAX/2;j>3;j--)
                         {
                             if((date[i][j+1]>date[i][j])&&(date[i][j+2]>date[i][j-1]))
                              {
                                  left=j;
								  left_flag=1;
                                  break;
                              }
							 left_flag=0;
                         }
                        left_line[left_dot][0]=i;left_line[left_dot][1]=left;left_dot++;
                        for(j=Y_MAX/2;j<Y_MAX-3;j++)
                         {
                             if((date[i][j-1]>date[i][j])&&(date[i][j-2]>date[i][j+1]))
                              {
                                  right=j;
								  right_flag=1;
                                  break;
                              }
							 right_flag=0;
                         }
                        right_line[right_dot][0]=i;right_line[right_dot][1]=right;right_dot++;
                    }
                   //先扫左边的一条线
                   ones_ok=0;error_times=0;
                   for(i;(i>2)&&(i<X_MAX-2)&&(j>2)&&(j<Y_MAX-2);)
                    {
                        if(scan_mode==0)//如果现在是行扫描
                         {
                             root1=left_line[left_dot-1][1];
                             if(root1<scale) left_start=2;
                             else left_start=root1-scale;
                             if(root1+scale>Y_MAX-2)  left_end=Y_MAX-2;
                             else left_end=root1+scale;
                             for(j=left_end;j>=left_start;j--)
                              {
                                  if((date[i][j+1]>date[i][j])&&(date[i][j+2]>date[i][j-1]))
                                   {
                                       left=j;
                                       left_flag=1;//表示采集到左边缘
                                       left_line[left_dot][0]=i;left_line[left_dot][1]=left;left_dot++;
                                       break;
                                   }
                                  left_flag=0;
                              }
                             if(left_flag==0)//v如果没有搜到，就看下一行是不是全为黑或者全白
                              {
                                  j=(left_end+left_start)/2;
                                  if((left_line[left_dot-1][1]-left_line[left_dot-6][1]>0)&&(left_line[left_dot-2][1]-left_line[left_dot-7][1]>0))
                                      lie_dir=1;//若连续两个都比前面大，说明在往列增大的地方偏
                                  else lie_dir=0;
                                  if(!ones_ok)
                                   {
							for(k=i;k<X_MAX;k+=2)
							 {
								 for(left_10_j=0;left_10_j<Y_MAX;left_10_j+=2)
								 {
									 if(date[k][left_10_j]!=254)
									 {
										 t=0;
										 break;
									 }
									 t=1;
								 }
                                 if(t==1) break;
								 
							 }//t=1表示遇到十字
                                   }
							if((t)&&(!ones_ok))//如果遇到十字
							{
								for(k;(k!=5)&&(k<X_MAX);k+=5)
								{
									left_10_j=2;
									for(left_10_j;left_10_j<Y_MAX-2;left_10_j+=2)
									{
										if((date[k+i][left_10_j+2]>date[k+i][left_10_j-2])&&(date[k+i][left_10_j+1]>date[k+i][left_10_j-1]))
										  {
											  left_10[1]=left_10_j;
											  left_10[0]=k;
											  k=0;
											  break;
										  }
									}
								}
								dot_sub=(left_10[0]-left_line[left_dot-3][0])/(left_10[1]-left_line[left_dot-3][1]);
								for(i;i<left_10[0];i++)
								{
								    left_line[left_dot][0]=i;
									if((left_10[0]-i)%dot_sub==0)
										left_line[left_dot][1]=left_line[left_dot-1][1]+1;
									else left_line[left_dot][1]=left_line[left_dot-1][1];
									left_dot++;
								}
								t=0;ones_ok=1;
							}
                            else  if((!t)&&((date[i+1][j-1]==date[i+1][j])&&(date[i+1][j]==date[i+1][j+1])&&(date[i+1][j]==1)&&(lie_dir))||((date[i+1][j-1]==date[i+1][j])&&(date[i+1][j]==date[i+1][j+1])&&(date[i+1][j]==254)&&(lie_dir==0)))//表示都是黑点或者白点
                                   {
                                       left_flag=0;
                                       scan_mode=1;
                                       i--;
                                   }
                                 //else  while(1);//否则先死在这,表示后面 不只是黑点，不应该是转换地点，可能是十字//待写
                              }
                             i++;
                         }
                        else 
                         {
                             root1=left_line[left_dot-1][0];
                             if(root1<scale) left_start=2;
                             else left_start=root1-scale;
                             if(root1+scale>X_MAX-2)  left_end=X_MAX-2;
                             else left_end=root1+scale;
                             if(lie_dir)
                              {
                                  for(i=left_start;i<left_end;i++)
                                   {
                                       if((date[i-1][j]>date[i][j])&&(date[i-2][j]>date[i+1][j]))
                                        {
                                            left=i;
                                            left_flag=1;//表示采集到左边缘
											error_times=0;
                                            left_line[left_dot][0]=left;left_line[left_dot][1]=j;left_dot++;
                                            break;
                                        }
                                       left_flag=0;
                                   }
								  if(left_flag=0)
								  {	
										left_line[left_dot][0]=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
										left_line[left_dot][1]=left_line[left_dot-1][1]-left_line[left_dot-2][1]+left_line[left_dot-1][1];
										left_dot++;
										 error_times++;
										 if( error_times==3) break;
								  }
                                  j++;
                              }
                             else 
                              {
                                  for(i=left_end;i>left_start;i--)
                                   {
                                       if((date[i+1][j]>date[i][j])&&(date[i+2][j]>date[i-1][j]))
                                        {
                                            left=i;
                                            left_flag=1;//表示采集到左边缘
											error_times=0;
                                            left_line[left_dot][0]=left;left_line[left_dot][1]=j;left_dot++;
                                            break;
                                        }
                                       left_flag=0;
                                   }
								  if(left_flag=0)
								  {	
										left_line[left_dot][0]=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
										left_line[left_dot][1]=left_line[left_dot-1][1]-left_line[left_dot-2][1]+left_line[left_dot-1][1];
										left_dot++;error_times++;
										 if( error_times==3) break;
								  }
                                  j--;
                              }
                         }
                    }
                   
                   //右边的一条线
                   scan_mode=0;error_times=0;
                   for(i=end,j=right_line[right_dot-1][1];(i>2)&&(i<X_MAX-2)&&(j>2)&&(j<Y_MAX-2);)
                    {
                        if(scan_mode==0)//如果现在是行扫描
                         {
                             root2=right_line[right_dot-1][1];
                             if(root2<scale) right_start=2;
                             else right_start=root2-scale;
                             if(root2+scale>Y_MAX-2)  right_end=Y_MAX-2;
                             else right_end=root2+scale;
                             for(j=right_start;j<right_end;j++)
                              {
                                  if((date[i][j-1]>date[i][j])&&(date[i][j-2]>date[i][j+1]))
                                   {
                                       right=j;
                                       right_flag=1;//表示采集到左边缘
                                       right_line[right_dot][0]=i;right_line[right_dot][1]=right;right_dot++;
                                       break;
                                   }
                                  right_flag=0;
                              }
                             if(right_flag==0)//v如果没有搜到，就看下一行是不是全为黑或者全白
                              {
                                  j=(right_end+right_start)/2;
                                  if((right_line[right_dot-1][1]-right_line[right_dot-6][1]>0)&&(right_line[right_dot-2][1]-right_line[right_dot-7][1]>0))
                                      lie_dir=1;//若连续两个都比前面大，说明在往列增大的地方偏
                                  else lie_dir=0;
                             if(((date[i+1][j-1]==date[i+1][j])&&(date[i][j]==date[i+1][j+1])&&(date[i][j]==254)&&lie_dir)||((date[i+1][j-1]==date[i+1][j])&&(date[i][j]==date[i+1][j+1])&&(date[i][j]==1)&&(lie_dir==0)))//表示都是黑点                                   {
                                   {
                                       right_flag=0;
                                       scan_mode=1;
                                       i--;
                                   }
                                 // else while(1);//否则先死在这,表示后面 不只是黑点，不应该是转换地点，可能是十字//待写
                              }
                             i++;
                         }
                        else 
                         {
                             root2=right_line[right_dot-1][0];
                             if(root2<scale) right_start=2;
                             else right_start=root2-scale;
                             if(root2+scale>X_MAX-2)  right_end=X_MAX-2;
                             else right_end=root2+scale;
                             if(lie_dir)
                              {
                                  for(i=right_end;i>right_start;i--)
                                   {
                                       if((date[i+1][j]>date[i][j])&&(date[i+2][j]>date[i-1][j]))
                                        {
                                            right=i;
                                            right_flag=1;//表示采集到左边缘
											error_times=0;
                                            right_line[right_dot][0]=right;right_line[right_dot][1]=j;right_dot++;
                                            break;
                                        }
                                       right_flag=0;
                                   }
                                  j++;
								  if(right_flag==0)
								  {
										right_line[right_dot][0]=right_line[right_dot-1][0]-right_line[right_dot-2][0]+right_line[right_dot-1][0];
										right_line[right_dot][1]=right_line[right_dot-1][1]-right_line[right_dot-2][1]+right_line[right_dot-1][1];
										right_dot++;error_times++;
											 if( error_times==3) break;
								  }
                              }
                             else 
                              {
                                  for(i=right_start;i<right_end;i++)
                                   {
                                       if((date[i-1][j]>date[i][j])&&(date[i-2][j]>date[i+1][j]))
                                        {
                                            right=i;
                                            right_flag=1;//表示采集到左边缘
											error_times=0;
                                            right_line[right_dot][0]=right;right_line[right_dot][1]=j;right_dot++;
                                            break;
                                        }
                                       right_flag=0;
                                   }
								    if(right_flag==0)
								  {
										right_line[right_dot][0]=2*right_line[right_dot-1][0]-right_line[right_dot-2][0];
										right_line[right_dot][1]=2*right_line[right_dot-1][1]-right_line[right_dot-2][1];
										right_dot++;
										error_times++;
											 if( error_times==3) break;
								  }
                                  j--;
                              }
                         }
                    }          
#endif
					
			//give up machine to display
			for(i=0;i<X_MAX;i++)
				for(j=0;j<Y_MAX;j++)
				{
					date[i][j]=date[i][j]+date[i][j]*256+date[i][j]*65536;
				}
				for(j=0;j<3;j++)
					{
					for(i=0;i<left_dot;i++)
					 {
							date[left_line[i][0]][left_line[i][1]+j]=red_line;
						}
					   for(i=0;i<right_dot;i++)
						{
							date[right_line[i][0]][right_line[i][1]+j]=blue_line;
						}
					}
				for(j=0;j<Y_MAX;j++)
					for(i=0;i<X_MAX/2;i++)
					{
						temp=date[i][j];
						date[i][j]=date[X_MAX-i-1][j];
						date[X_MAX-i-1][j]=temp;
					}
				for(i=0;i<X_MAX;i++)
					for(j=0;j<Y_MAX/2;j++)
					{
						temp=date[i][j];
						date[i][j]=date[i][Y_MAX-j-1];
						date[i][Y_MAX-j-1]=temp;
					}
}