/*****************************************************************
* @file    : IIC_IO.c 
* @brief   : 
* @author  : Dtb 
* @version : 
* @date    : 2023-7-31 
*****************************************************************/

#include "latency.h"
#include "S32K144.h"
#include "IIC_IO.h"


//IIC引脚初始化
void IO_IIC_init(IIC_type *IICx, uint8_t _x, uint32_t SDA_pin, uint32_t SCL_pin, uint8_t dl_time) {
    switch (_x)
    { // ******初始化IO配置 *************/
    case 'A':IICx->PTx = PTA; IICx->PORTx = PORTA; IICx->PCC_PORTx_INDEX = PCC_PORTA_INDEX; break;
    case 'B':IICx->PTx = PTB; IICx->PORTx = PORTB; IICx->PCC_PORTx_INDEX = PCC_PORTB_INDEX; break;
    case 'C':IICx->PTx = PTC; IICx->PORTx = PORTC; IICx->PCC_PORTx_INDEX = PCC_PORTC_INDEX; break;
    case 'D':IICx->PTx = PTD; IICx->PORTx = PORTD; IICx->PCC_PORTx_INDEX = PCC_PORTD_INDEX; break;
    case 'E':IICx->PTx = PTE; IICx->PORTx = PORTE; IICx->PCC_PORTx_INDEX = PCC_PORTE_INDEX; break;
    }
    IICx->dl_time = dl_time;
    IICx->SDA_pin = SDA_pin;
    IICx->SCL_pin = SCL_pin;
    PCC-> PCCn[IICx->PCC_PORTx_INDEX] |= PCC_PCCn_CGC_MASK; // 开启时钟模块
    IICx->PORTx->PCR[SDA_pin] = PORT_PCR_MUX(1) | PORT_PCR_DSE(1);   // 配置SDA引脚为GPIO，并设置驱动能力 1->强驱动, 0->普通驱动
    IICx->PORTx->PCR[SCL_pin] = PORT_PCR_MUX(1) | PORT_PCR_DSE(1);   // 配置SCL引脚为GPIO，并设置驱动能力
    IICx->PTx->PDDR |= (1 << SDA_pin);   //设置SDA_pin为输出模式
    IICx->PTx->PDDR |= (1 << SCL_pin);   //设置SCL_pin为输出模式
    SDA_Set(IICx); // SDA输出高电平
    SCL_Set(IICx); // SCL输出高电平
}
// IIC延时
void IIC_delay(uint8_t t){
	// while(t--);
    later_us(t);
}
 //设置SDA_pin为输出模式
void SDA_out(IIC_type *IICx){      
    IICx->PTx->PDDR |= (1 << IICx->SDA_pin);
}
//设置SDA_pin为输入模式
void SDA_in(IIC_type *IICx){        
    IICx->PTx->PDDR &= ~(1 << IICx->SDA_pin);
}
// SCL输出低电平
void SCL_Clr(IIC_type *IICx) { 
    IICx->PTx->PCOR |= (1UL << (IICx->SCL_pin)); 
}
// SCL输出高电平
void SCL_Set(IIC_type *IICx) {
    IICx->PTx->PSOR |= (1UL << (IICx->SCL_pin)); 
}
// SDA输出低电平
void SDA_Clr(IIC_type *IICx) {
    IICx->PTx->PCOR |= (1UL << (IICx->SDA_pin)); 
}
// SDA输出高电平
void SDA_Set(IIC_type *IICx) {
    IICx->PTx->PSOR |= (1UL << (IICx->SDA_pin)); 
}
bool IIC_read_SDA(IIC_type *IICx){  //读SDA口状态
    return(IICx->PTx->PDIR & (1<<IICx->SDA_pin));
}

//产生IIC起始信号
void IIC_start(IIC_type *IICx) {
    SDA_out(IICx); //设置SDA_pin为输出模式
    SCL_Set(IICx); // SCL输出高电平
    SDA_Set(IICx); // SDA输出高电平 // 空闲态
    IIC_delay(IICx->dl_time);
    SDA_Clr(IICx); // 当SCL线为高电平时，SDA线上由高到低跳变 即为开始信号
    IIC_delay(IICx->dl_time);
    SCL_Clr(IICx); // SCL输出低电平 // 钳住，等待发送或接受
    IIC_delay(IICx->dl_time);
}

//产生IIC停止信号 //IIC空闲时SCL和SDA均输出高电平，这样不会干扰其他设备的收发
void IIC_stop(IIC_type *IICx) {
	SDA_out(IICx); //设置SDA_pin为输出模式
    SCL_Set(IICx); // SCL输出高电平
    SDA_Clr(IICx); // SDA输出低电平
 	IIC_delay(IICx->dl_time);
    SDA_Set(IICx); // 当SCL线为高电平时，SDA线上由低到高跳变 即为结束信号
	IIC_delay(IICx->dl_time);
}

//等待应答信号到来 返回值： 1：接收应答失败  0：接收应答成功
bool IIC_wait_Ack(IIC_type *IICx) {
	uint8_t errCount =0;
	SDA_in(IICx);  //SDA设置为输入
    SDA_Set(IICx); // SDA输出高电平
    IIC_delay(IICx->dl_time);
    SCL_Set(IICx); // SCL输出高电平
    IIC_delay(IICx->dl_time);
	while(IIC_read_SDA(IICx)) { //读SDA口状态 应答需为低电平
		errCount++;
		if(errCount>5) {      //读了多次SDA口均为高电平，则接收应答失败
			IIC_stop(IICx);
			return 1;
		}
	}
	SCL_Clr(IICx); // SCL时钟输出低电平0
	return 0;
}

//产生ACK应答 //SCL为高电平时SDA为低电平表示应答
void IIC_Ack(IIC_type *IICx) {
	SCL_Clr(IICx); // SCL时钟输出低电平0
	SDA_out(IICx); // 设置SDA_pin为输出模式
	SDA_Clr(IICx); // SDA输出低电平
	IIC_delay(IICx->dl_time);
	SCL_Set(IICx); // SCL输出高电平1
	IIC_delay(IICx->dl_time);
	SCL_Clr(IICx); // SCL时钟输出低电平0
    IIC_delay(IICx->dl_time);
    SDA_Set(IICx); // SDA输出高电平 释放SDA
}
//不产生ACK应答
void IIC_NAck(IIC_type *IICx) {
	SCL_Clr(IICx); // SCL时钟输出低电平0
	SDA_out(IICx); // 设置SDA_pin为输出模式
	SDA_Set(IICx); // SDA输出高电平
	IIC_delay(IICx->dl_time);
	SCL_Set(IICx); // SCL输出高电平1
	IIC_delay(IICx->dl_time);
	SCL_Clr(IICx); // SCL时钟输出低电平0
    IIC_delay(IICx->dl_time);
}

//IIC发送一个字节
void IIC_Send_Byte(IIC_type *IICx, uint8_t byte_data) {
    uint8_t t;
	SDA_out(IICx);                                   // 设置SDA_pin为输出模式
    SCL_Clr(IICx);     //拉低时钟开始数据传输
    for(t=0;t<8;t++) {
        // 从最高位开始发送
        (byte_data & 0x80) ? (SDA_Set(IICx)) : (SDA_Clr(IICx));
        byte_data <<= 1; 	                         // 更新最高位的数
		IIC_delay(IICx->dl_time);
		SCL_Set(IICx); // SCL输出高电平1
		IIC_delay(IICx->dl_time);
		SCL_Clr(IICx); // SCL时钟输出低电平0
		IIC_delay(IICx->dl_time);
    }
    SDA_Set(IICx);     // SDA输出高电平//释放SDA
}

//IIC读1个字节，ack=1时，发送ACK，ack=0，发送NACK
uint8_t IIC_Read_Byte(IIC_type *IICx, uint8_t ack) {
	uint8_t i, receive=0;
	SDA_in(IICx);                                    //SDA设置为输入
    for(i=0;i<8;i++ ) {
        SCL_Clr(IICx); // SCL时钟输出低电平0
        IIC_delay(IICx->dl_time);
		SCL_Set(IICx); // SCL输出高电平1
        receive <<= 1;                               //将最低位空出
        if(IIC_read_SDA(IICx)) receive++;            //读到高电平，则最低位为1
		IIC_delay(IICx->dl_time);
    }
    if (!ack)
        IIC_NAck(IICx);//发送nACK
    else
        IIC_Ack(IICx); //发送ACK
    return receive;
}


