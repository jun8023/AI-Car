#include "all.h"
#include "Catch.h"

uint8   date[X_MAX+5][Y_MAX+15],center[dot_num][2];
extern float input=0,input_error=0,output=0,p=7;
uint8 OK_FLAG=0;
uint8 root1=0,root2=0,change_left=0,change_right=0;
uint8 line_left=0,line_right=0,left_start,right_start,left_end,right_end;
uint8 left=0,right=0,left_flag=0,right_flag=0,left_last,right_last;
uint32 dif=0;
//��ֵ�˲�����
//����ɨ���㷨
uint8 left_line[200][2],right_line[200][2],left_dot=0,right_dot=0,left_10[2],right_10[2],left_10_i,left_10_j,right_10_i,right_10_j,ones_ok=0,ten_hang,ten_lie,ten_left_ok=0,ten_right_ok=0;
int8 dot_sub_left=0,dot_sub_right=0;
uint8 scan_mode_left=0,scan_mode_right=0,type=0;//������������;//0��ʾ��ɨ�裬1��ʾ��ɨ��
uint8 left_h_dir=0,left_l_dir=0,right_h_dir=0,right_l_dir=0;//�趨ת��ʱɨ�跽��  С��0��ʾ���ݼ�  1��ʾ������
uint8 error_times_left=0,error_times_right=0,left_i=0,left_j=0,right_i=0,right_j=0,hang_dir=0,lie_dir=0,left_end_flag=1,right_end_flag=1;//0��ʾ������
uint32 temp;

float test=0;       
//ֱ�ǲ���
uint8 angle_ok = 0, angle_hang, angle_lie,angle_left_ok = 0, angle_right_ok = 0,left_angle_i,left_angle_j,right_angle_i,right_angle_j,left_angle[2],right_angle[2];
//���߲���
uint8 test_single_line = 0, single_left[2], single_right[2], single_left_ok, single_right_ok,start_single=0;//���߼��







void CAM_get()
 {
     uint8 i,j;uint8 successed=1;
     uint8 temp1,temp2,temp3;
     #if    1        //��ֵ��
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
  #if     1           //��Եɨ���㷨		//������ʼ��
                   scan_mode_left=scan_mode_right=0;//0��ʾ��ɨ�裬1��ʾ��ɨ��
				   left_dot=0;right_dot=0;		//�ɵ������ҵ���
                    left_h_dir=0;left_l_dir=0;		//����ǰ������
					right_h_dir=0;right_l_dir=0;		//����ǰ������
					left_flag=0,right_flag=0;	//�Ƿ�ɼ������־λ
					left_i=right_i=0;
					left_j=right_j=0;
					type=0;//������������
					left_end_flag=1,right_end_flag=1;//ɨ������0
                   for(left_i=right_i=start;!(((left_flag==1)&&(right_flag==1))&&(left_i>=end)&&(right_i>=end));)   //��ɨǰ���У�ȷ����Ч�߽�
                    {
                        left=0;right=Y_MAX;
						if((!left_flag)||(left_i<=end))
						{
							for(j=Y_MAX/2;j>3;j--)
							 {
								 if((date[left_i][j-2]==1)&&(date[left_i][j-1]==1))
								  {
									  left=j;
									  left_flag=1;
									  break;
								  }
								 left_flag=0;
							 }
							left_line[left_dot][0]=left_i;left_line[left_dot][1]=left;left_dot++;left_i++;
						}
						if((!right_flag)||(right_i<=end))
						{
							for(j=Y_MAX/2;j<Y_MAX-3;j++)
							 {
								 if((date[right_i][j+1]==1)&&(date[right_i][j+2]==1))
								  {
									  right=j;
									  right_flag=1;
									  break;
								  }
								 right_flag=0;
							 }
							right_line[right_dot][0]=right_i;right_line[right_dot][1]=right;right_dot++;right_i++;
						}
                        
                    }

						//��������
					for(left_i,right_i,left_j=left_line[left_dot-1][1],right_j=right_line[right_dot-1][1];left_end_flag||right_end_flag;)
					{
						//����ɨ��ȷ����������
						if(change_right&change_left)//ȷ��ɨ�跽ʽ
						{
							change_right=change_left=0;//һ��ͼƬֻ����һ��
							if((date[left_line[left_dot-1][0]+2][left_line[left_dot-1][1]-1]==254)||(date[left_line[left_dot-1][0]+2][left_line[left_dot-1][1]]==254))
							{
								left_l_dir=0;//���С����ɨ
							}
							else left_l_dir=1;//��������ɨ
							if((date[right_line[right_dot-1][0]+2][right_line[right_dot-1][1]-1]==254)||(date[right_line[right_dot-1][0]+2][right_line[right_dot-1][1]]==254))
							{
								right_l_dir=1;//��������ɨ
							}
							else right_l_dir=0;//���С����ɨ
							if(left_l_dir&right_l_dir) type=1;//����ֱ�ǻ������������ƫ
							else if(!(left_l_dir||right_l_dir)) type=2;//����ֱ�ǻ���������Сƫ
							else if((!left_l_dir)&&(right_l_dir)) type=3;//��������ʮ��
							else type=4;//��������ֱ��
							scan_mode_left=scan_mode_right=1;//��ʼ����ɨ
							left_j=left_line[left_dot-1][1];
							right_j=right_line[right_dot-1][1];
						}
							if(!(scan_mode_left|scan_mode_right))//����������ɨ�跽ʽ
							{
								if(!change_left)//����Ҳ�����ֹͣ
								{
									root1=left_line[left_dot-1][1];
									//	root1=left_line[left_dot-1][1]-left_line[left_dot-2][1]+left_line[left_dot-1][1];
									if(date[left_i][root1]==254)//��������Ǹ�Ϊ�׵㣬������
									{
										if(root1<scale) left_end=3; else left_end=root1-scale;
										for(left_j=root1;left_j>=left_end;left_j--)
										{
											if((date[left_i][left_j-2]==1)&&(date[left_i][left_j-1]==1))
											   {
												   left=left_j;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left_i;left_line[left_dot][1]=left;left_dot++;
												   break;
											   }
											left_flag=0;
										}
										if(left_flag==0) error_times_left++;
										if(error_times_left>=1) change_left=1;
									}
									else 
									{
										if(root1+scale>Y_MAX-3) left_end=Y_MAX-3; else left_end=root1+scale;
										for(left_j=root1;left_j<left_end;left_j++)
										{
											if((date[left_i][left_j+2]==254)&&(date[left_i][left_j+1]==254))
											   {
												   left=left_j+1;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left_i;left_line[left_dot][1]=left;left_dot++;
												   break;
											   }
											left_flag=0;
										}
											if(left_flag==0) error_times_left++;
											if(error_times_left>=1) change_left=1;
									}
									left_i++;			
                                }
								if(!change_right)//�ұ��� ������ֹͣ
								{
								root2=right_line[right_dot-1][1];
							//	root2=right_line[right_dot-1][1]-right_line[right_dot-2][1]+right_line[right_dot-1][1];
								if(date[right_i][root2]==254)//��������Ǹ�Ϊ�׵㣬������
								{
									if(root2+scale>Y_MAX-3) right_end=Y_MAX-3; else right_end=root2+scale;
									for(right_j=root2;right_j<right_end;right_j++)
									{
										if((date[right_i][right_j+2]==1)&&(date[right_i][right_j+1]==1))
										   {
											   right=right_j;
											   error_times_right=0;
											   right_flag=1;//��ʾ�ɼ����ұ�Ե
											   right_line[right_dot][0]=right_i;right_line[right_dot][1]=right;right_dot++;
											   break;
										   }
										right_flag=0;
									}
									if(right_flag==0) error_times_right++;if(error_times_right>=1) change_right=1;
								}
								else 
								{
									if(root2<scale) right_end=3; else right_end=root2-scale;
									for(right_j=root2;right_j>right_end;right_j--)
									{
										if((date[right_i][right_j-2]==254)&&(date[right_i][right_j-1]==254))
										   {
											   right=right_j-1;
											   error_times_right=0;
											   right_flag=1;//��ʾ�ɼ������Ե
											   right_line[right_dot][0]=right_i;right_line[right_dot][1]=right;right_dot++;
											   break;
										   }
										right_flag=0;
									}
									if(right_flag==0) error_times_right++;if(error_times_right>=1) change_right=1;
								}
                                 right_i++;
								}
							}
						switch(type)		//˵��������֮һ��ִ����break;
						{
						case 1:
							{
								if(left_end_flag)	//���û���꣬������
								{
									root1=left_line[left_dot-1][0];
								//	root1=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
									if(date[root1][left_j]==254)//��������Ǹ�Ϊ�׵㣬������
									{
										if(root1<scale) left_end=3; else left_end=root1+scale;
										for(left_i=root1;left_i<left_end;left_i++)
										{
											if((date[left_i+2][left_j]==1)&&(date[left_i+1][left_j]==1))
											   {
												   left=left_i;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left;left_line[left_dot][1]=left_j;left_dot++;
												   break;
											   }
											left_flag=0;
										}
									}
									else 
									{
										if(root1+scale>Y_MAX-3) left_end=Y_MAX-3; else left_end=root1-scale;
										for(left_i=root1;left_i>left_end;left_i--)
										{
											if((date[left_i-2][left_j]==254)&&(date[left_i-1][left_j]==254))
											   {
													left=left_i-1;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left;left_line[left_dot][1]=left_j;left_dot++;
												   break;
											   }
											left_flag=0;
										}
									}
								left_j++;
								}
								if(right_end_flag)//�ұ�û���꣬������
								{
									root2=right_line[right_dot-1][0];
								//	root2=right_line[right_dot-1][0]-right_line[right_dot-2][0]+right_line[right_dot-1][0];
									if(date[root2][right_j]==1)//��������Ǹ�Ϊ�ڵ㣬������
									{
										if(root2<scale) right_end=3; else right_end=root2+scale;
										for(right_i=root2;right_i<right_end;right_i++)
										{
											if((date[right_i+2][right_j]==254)&&(date[right_i+1][right_j]==254))
											   {
												   right=right_i+1;
												   error_times_right=0;
												   right_flag=1;//��ʾ�ɼ������Ե
												   right_line[right_dot][0]=right;right_line[right_dot][1]=right_j;right_dot++;
												   break;
											   }
											right_flag=0;
										}
									}
									else 
									{
										if(root2+scale>Y_MAX-3) right_end=Y_MAX-3; else right_end=root2-scale;
										for(right_i=root2;right_i>right_end;right_i--)
										{
											if((date[right_i-2][right_j]==1)&&(date[right_i-1][right_j]==1))
											   {
													right=right_i;
												   error_times_right=0;
												   right_flag=1;//��ʾ�ɼ������Ե
												   right_line[right_dot][0]=right;right_line[right_dot][1]=right_j;right_dot++;
												   break;
											   }
											right_flag=0;
										}
									}
								right_j++;
								}
							}break;
						case 2:
							{
								if(left_end_flag)	//���û���꣬������
								{
									root1=left_line[left_dot-1][0];
								//	root1=left_line[left_dot-1][0]-left_line[left_dot-2][0]+left_line[left_dot-1][0];
									if(date[root1][left_j]==254)//��������Ǹ�Ϊ�׵㣬������
									{
										if(root1<scale) left_end=3; else left_end=root1-scale;
										for(left_i=root1;left_i>left_end;left_i--)
										{
											if((date[left_i-2][left_j]==1)&&(date[left_i-1][left_j]==1))
											   {
												   left=left_i;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left;left_line[left_dot][1]=left_j;left_dot++;
												   break;
											   }
											left_flag=0;
										}
									}
									else 
									{
										if(root1+scale>Y_MAX-3) left_end=Y_MAX-3; else left_end=root1+scale;
										for(left_i=root1;left_i<left_end;left_i++)
										{
											if((date[left_i+2][left_j]==254)&&(date[left_i+1][left_j]==254))
											   {
													left=left_i+1;
												   error_times_left=0;
												   left_flag=1;//��ʾ�ɼ������Ե
												   left_line[left_dot][0]=left;left_line[left_dot][1]=left_j;left_dot++;
												   break;
											   }
											left_flag=0;
										}
									}
								left_j--;
								}
								if(right_end_flag)//�ұ�û���꣬������
								{
									root2=right_line[right_dot-1][0];
								//	root2=right_line[right_dot-1][0]-right_line[right_dot-2][0]+right_line[right_dot-1][0];
									if(date[root2][right_j]==1)//��������Ǹ�Ϊ�ڵ㣬������
									{
										if(root2<scale) right_end=3; else right_end=root2-scale;
										for(right_i=root2;right_i>right_end;right_i--)
										{
											if((date[right_i-2][right_j]==254)&&(date[right_i-1][right_j]==254))
											   {
												   right=right_i-1;
												   error_times_right=0;
												   right_flag=1;//��ʾ�ɼ������Ե
												   right_line[right_dot][0]=right;right_line[right_dot][1]=right_j;right_dot++;
												   break;
											   }
											right_flag=0;
										}
									}
									else 
									{
										if(root2+scale>Y_MAX-3) right_end=Y_MAX-3; else right_end=root2+scale;
										for(right_i=root2;right_i<right_end;right_i++)
										{
											if((date[right_i+2][right_j]==1)&&(date[right_i+1][right_j]==1))
											   {
													right=right_i;
												   error_times_right=0;
												   right_flag=1;//��ʾ�ɼ������Ե
												   right_line[right_dot][0]=right;right_line[right_dot][1]=right_j;right_dot++;
												   break;
											   }
											right_flag=0;
										}
									}
								right_j--;
								}
							}break;
						case 3:{
                                    ones_ok=0;ten_left_ok=ten_right_ok=0;
									ten_hang=(left_line[left_dot-1][0]+right_line[right_dot-1][0])/2+15;//������ɨ10��
									ten_lie=(left_line[left_dot-1][1]+right_line[right_dot-1][1])/2;
									while(!ones_ok)//�жϲ����Ƿ�ɹ�
									{
										for(ten_hang;(!(ten_left_ok&ten_right_ok))&&(ten_hang<X_MAX-3);ten_hang+=5)
										{
											for(left_10_j=ten_lie;(!ten_left_ok)&&(left_10_j>3);left_10_j-=2)
											{
												if((date[ten_hang][left_10_j-2]==1)&&(date[ten_hang][left_10_j-1]==1))
												  {
													  
													  ten_left_ok=1;
													  left_10[1]=left_10_j;
														left_10[0]=ten_hang;
													  break;
												  }
												ten_left_ok=0;
											}
											
											for(right_10_j=ten_lie;(right_10_j<Y_MAX-3)&&(!ten_right_ok);right_10_j++)
											{
												if((date[ten_hang][right_10_j+2]==1)&&(date[ten_hang][right_10_j+1]==1))
												  {
													 
													  ten_right_ok=1;
													 right_10[1]=right_10_j;
													 right_10[0]=ten_hang;
													  break;
												  }
												ten_right_ok=0;
											}
										}//ʮ��û�в��ߵı�Ҫ���ܵ�ʱ�����´�����Ժ���
										ones_ok=1;//����һ���Թ�
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
										
										//����������ҵ�~��ֱ�����¿�ʼ
										if(ten_left_ok&ten_right_ok)//�ѵ����¶�λɨ��Ȼ���ֱ�����һ������
										{
											type=0;scan_mode_left=scan_mode_right=0;
											left_line[left_dot][0]=left_i=left_10[0];left_line[left_dot][1]=left_j=left_10[1];left_dot++;
											right_line[left_dot][0]=right_i=right_10[0];right_line[right_dot][1]=right_j=right_10[1];right_dot++;
										}
										else
										{
											right_i=left_i=(left_line[left_dot-1][0]+right_line[right_dot-1][0])/2;//������ɨ10��
											right_j=left_j=(left_line[left_dot-1][1]+right_line[right_dot-1][1])/2;
											for(left_i;left_i<=X_MAX-2;left_i++)
												{
													left_line[left_dot][0]=left_i;left_line[left_dot][1]=left_j;left_dot++;
												}
											for(right_i;right_i<=X_MAX-2;right_i++)
												{
													right_line[left_dot][0]=right_i;right_line[right_dot][1]=right_j;right_dot++;
												}
										}
									}
							   }break;
						case 4: {
							ones_ok = 0; angle_left_ok = angle_right_ok = 0;
							angle_hang = (left_line[left_dot - 1][0] + right_line[right_dot - 1][0]) / 2 + 10;//������ɨ10��
							angle_lie = (left_line[left_dot - 1][1] + right_line[right_dot - 1][1]) / 2;
							while (!ones_ok)//�жϲ����Ƿ�ɹ�
							{
								for (angle_hang; angle_hang < Y_MAX - 3; angle_hang += 5)
								{
									if ((date[angle_hang][angle_lie] == 254) && (date[angle_hang][angle_lie + 1] == 254))//�����ѵ�����֮��İ���
									{	
										angle_ok = 1;
										break;
									}
									angle_ok = 0;
								}
								ones_ok = 1;//����������һ�Σ��˳�
								if (angle_ok)	//������������ߣ��ұ߽��˳���ֱ��ģʽ
								{
										for (left_angle_j = angle_lie; (!angle_left_ok) && (left_angle_j>3); left_angle_j -= 2)
										{
											if ((date[angle_hang][left_angle_j - 2] == 1) && (date[angle_hang][left_angle_j - 1] == 1))
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
											if ((date[angle_hang][right_angle_j + 2] == 1) && (date[angle_hang][right_angle_j + 1] == 1))
											{
												angle_left_ok=1;
												right_angle[1] = right_angle_j;
												right_angle[0] = angle_hang;
												break;
											}
											angle_right_ok = 0;
										}
										type = 0; scan_mode_left = scan_mode_right = 0;	//׼��ֱ������
										left_line[left_dot][0] = left_i= left_angle[0]; left_line[left_dot][1] = left_j=left_10_j= left_angle[1]; left_dot++;
										right_line[left_dot][0] = right_i= right_angle[0]; right_line[right_dot][1] = right_j= right_angle[1]; right_dot++;
									}
								else         //����ֱ�Ӳ���
								{
									angle_hang = (left_line[left_dot - 1][0] + right_line[right_dot - 1][0]) / 2;//������ɨ10��
									angle_lie = (left_line[left_dot - 1][1] + right_line[right_dot - 1][1]) / 2;
									for (left_i; left_i <= X_MAX - 2; left_i++)
									{
										left_line[left_dot][0] = left_i; left_line[left_dot][1] = left_j; left_dot++;
										right_line[left_dot][0] = left_i; right_line[right_dot][1] = left_j; right_dot++;
									}
								}
							}
						}  break;
						}
						left_end_flag=((left_i>3)&&(left_i<X_MAX-3)&&(left_j>3)&&(left_j<Y_MAX-3));//���˱߽磬��0
						right_end_flag=((right_i>3)&&(right_i<X_MAX-3)&&(right_j>3)&&(right_j<Y_MAX-3));
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