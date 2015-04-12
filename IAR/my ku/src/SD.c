#include "all.h"
#include "SD.h"

volatile u8 sd_type;



static void sd_Disable_Select( void )
{
// SD_CS_SET();    // 取消片选
 sd_ReadWriteByte( 0xff ); // 补八个时钟
}


static u8 sd_WaitResponse( u8 response )
{
	u16 cnt = 0xffff;
	do 
	{
		if ( sd_ReadWriteByte( 0xff ) == response ) // 判断指定的应答是否出现
		{
			return 0;
		}
		cnt--;
	} while ( cnt );
	//printf( "等待 0x%x 信号失败....\n\n", response );
	return 1;
}


static u8  sd_WaitRead( void )
{
	u32 cnt = 0x00ffffff;
	u8  sta;
	do 
	{
		sta = sd_ReadWriteByte( 0xff );
		if ( sta == 0xff ) // 判断等待读取是否就绪
		{
			return 0;
		}
		cnt--;
	} while ( cnt );
	return 1;
}

static u8 sd_WriteBufferToDisk( u8 *buff, u8 cmd )
{
	u16 cnt;
	u8 sta;
	if ( sd_WaitRead() )
	{
		//  printf( "数据写入等待失败....\n\n\n" );
	return 1;
	}
	sd_ReadWriteByte( cmd );
	if ( cmd != 0xFD )
	{
		for ( cnt = 0; cnt < 512; cnt++ )
		{
			sd_ReadWriteByte( *buff );
			buff++;
		}
		for ( cnt = 0; cnt < 2; cnt++ )
		{
			sd_ReadWriteByte( 0xff );
		}
		sta = sd_ReadWriteByte( 0xff );
		if ( ( sta & 0x1F ) != 0x05 )
		{
			//  printf( "数据完成，但发生错误，错误码是 %x  \n\n\n", sta );
			return 2;
		}
	}
	return 0;
}


u8 sd_send_cmds( u8 cmd, u32 arg, u8 crc )
{
       u32 cnt=512;
       u8  sta;
       
  /*     sd_Disable_Select();  // 先取消片选
       if ( sd_Enable_Select() ) // 再使能片选
       {
        return 0xff;   // 如果片选失败，退出返回255
       }*/
       
       SPIx_ReadWriteByte( cmd | 0x40 );  // 发送命令组合
       SPIx_ReadWriteByte( (u8)(arg>>24) ); // 参数1
       SPIx_ReadWriteByte( (u8)(arg>>16) ); // 参数2
       SPIx_ReadWriteByte( (u8)(arg>>8) ); // 参数3
       SPIx_ReadWriteByte( (u8)(arg) );  // 参数4
       SPIx_ReadWriteByte( crc );   // CRC校验码

       do 
       {
        sta = SPIx_ReadWriteByte( 0xff ); // 读取发送状态
        cnt--;
       } while ( (cnt)&&(sta==0xff) );

       return sta; // 返回发送状态    
}

u8 SD_WriteMultiSectors( u8 *buff, u32 sectors, u32 num )
{
	u8 sta,rtn;
	u16 cnt=0xffff;
	if ( sd_type != SD_TYPE_V2HC )
	{
		sectors *= 512;
	}
	if ( num == 1 )
	{
		sta = sd_send_cmds( SD_CMD24, sectors, 0x01 );
		if ( sta == 0 )
		{
			sta = sd_WriteBufferToDisk( buff, 0xFE );
		}
	}else{
		if ( sd_type != SD_TYPE_MMC )
		{
			sd_send_cmds( SD_CMD55, 0, 0x01 );
			sd_send_cmds( SD_CMD23, num, 0x01 );
		}
		sta = sd_send_cmds( SD_CMD25, sectors, 0x01 );
		if ( sta == 0 )
		{
			do 
			{
				rtn = sd_WriteBufferToDisk( buff, 0xFC );
				buff += 512;
				num--;
			} while ( (sta==0)&&(num) );
			rtn = sd_WriteBufferToDisk( buff, 0xfd );
		}
	}
	do 
    {
    	sta = SPIx_ReadWriteByte( 0xff ); // 读取发送状态
        cnt--;
    } while ( (cnt)&&(sta!=0xff) );
//	sd_Disable_Select();
	return rtn;
}

static u8 sd_ReceiveDataToBuffer( u8 *buff, u16 lenght )
{
	u8 cnt;
	// 等待令牌 0xfe 回应
	if ( sd_WaitResponse( 0xfe ) )
	{
		//printf( "读取数据时等待令牌 0xFE 失败....\n\n" );
		return 1;
	}
	while ( lenght-- )
	{
		*buff = sd_ReadWriteByte( 0xff );
		buff++;
	}
	// 不进行CRC校验，发送假的CRC
	for ( cnt = 0; cnt < 2; cnt++ )
	{
		sd_ReadWriteByte( 0xff );
	}
 
	return 0;
}

u8 SD_ReadSingleSectors( u8 *buff, u32 sectors )
{
	u8 sta;
//	#if SD_DEBUG_EN>0
//	printf( "\n要读取的扇区是%d\n\n", sectors );
//	#endif
	if ( sd_type != SD_TYPE_V2HC )
	{ // 如果不是2.0HC高速卡，则转换为字节地址 
		sectors <<= 9; 
	}

	sta = sd_send_cmds( SD_CMD17, sectors, 0x01 );
	if ( sta == 0 )
	{
		sta = sd_ReceiveDataToBuffer( buff, 512 );
	}
	sd_Disable_Select();
	return sta;
}

u8 SD_ReadMultiSectors( u8 *buff, u32 sectors, u32 num )
{
	u8 sta;

	if ( sd_type != SD_TYPE_V2HC )
	{ // 如果不是2.0HC高速卡，则转换为字节地址 
		sectors <<= 9;
//		#if SD_DEBUG_EN>0
//			printf( "使用的不是高速卡哦！" );
//		#endif
	}

	if ( num == 1 )
	{
		sta = sd_send_cmds( SD_CMD17, sectors, 0x01 );
		if ( sta == 0 )
		{
			sta = sd_ReceiveDataToBuffer( buff, 512 );
		}
	}else{
		sta = sd_send_cmds( SD_CMD18, sectors, 0x01 );
		do 
		{
			sta = sd_ReceiveDataToBuffer( buff, 512 );
			buff += 512;
			num--;
		} while ( (sta==0)&&num );
		sd_send_cmds( SD_CMD12, 0, 0x01 );
	}
	sd_Disable_Select();
	return sta;
}

u8 SD_Init(void)
{
	u8 cnt,sta,rtn=0;
	u8  i,x;
	u8  buf[4];
	SYSDelay_ms(5);
	for ( cnt = 0; cnt < 24; cnt++ )
	{ 
		x=SPIx_ReadWriteByte(0xff);
	}
	cnt = 200;
   
	do 
	{ // 循环读取发送命令0的返回状态，直到出现0x01或是超出200次
		sta = sd_send_cmds( 0, 0, 0x95 ); 
		cnt--;
	} while ( ( sta != 0x01 ) && ( cnt ) );
     
	sta = sd_send_cmds( SD_CMD8, 0x1AA, 0x87 );  

	// 读取命令8后卡发送来的四字节数据，只有2.0版本有
	for ( i = 0; i < 4; i++ )
	{
		buf[ i ] = sd_ReadWriteByte( 0xff );
	}

    // 根据读取回来的四字节数据，判断该卡是否支持2.7---3.6的电压
	if ( ( buf[ 2 ] == 0x01 )&&( buf[ 3 ] == 0xAA ) ) // 
    {
		// 只有支持2.7---3.6电压了才继续操作
		cnt = 0xffff;
    }
	else
	{
		cnt = 0xffff;
		rtn = 1;
	}
		
    
    do 
	{
		sd_send_cmds( SD_CMD55, 0, 0x01 );
		sta = sd_send_cmds( SD_CMD41, 0x40000000, 0x01 );
		cnt--;
	} while ( sta && cnt );
	
	sta = sd_send_cmds( SD_CMD58, 0, 0 );

	if ( sta != 0x00 ) // 如果没有得到正确的回应
	{
//         sd_Disable_Select();
		sta = 1;
		rtn = 1;
	}

	// 如果有得到正确的回应，则读取卡发回来的OCR信息
	for ( i = 0; i < 4; i++ ) // 读取四字节的信息
	{
		buf[ i ] = sd_ReadWriteByte( 0xff );
	}
	if ( buf[0] & 0x40 )
	{
		//printf( "检测到卡的类型是V2.0HC高速卡,可以正常使用.....\n\n" );
		sd_type = SD_TYPE_V2HC;
	}else{
		//printf( "检测到卡的类型是V2.0普通卡,可以正常使用.....\n\n" );
		sd_type = SD_TYPE_V2;
	}
	return rtn;
}