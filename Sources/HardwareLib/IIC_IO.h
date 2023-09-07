/*
 * IIC_IO.h
 *
 *  Created on: 2023年7月31日
 *      Author: dengtongbei
 */

#ifndef IIC_IO_H_
#define IIC_IO_H_
#include "S32K144.h"
#include <stdbool.h>

typedef struct {
    GPIO_Type * PTx;
    PORT_Type * PORTx;
    uint8_t dl_time;  
    uint16_t PCC_PORTx_INDEX;
    uint32_t SDA_pin;
    uint32_t SCL_pin;
}IIC_type;

void IIC_delay(uint8_t t);// IIC延时
void IO_IIC_init(IIC_type *IICx, uint8_t _x, uint32_t SDA_pin, uint32_t SCL_pin, uint8_t dl_time);
void SDA_out(IIC_type *IICx);                           //设置SDA_pin为输出模式
void SDA_in(IIC_type *IICx);                            //设置SDA_pin为输入模式
void SCL_Clr(IIC_type *IICx);                           // SCL输出低电平
void SCL_Set(IIC_type *IICx);                           // SCL输出高电平
void SDA_Clr(IIC_type *IICx);                           // SDA输出低电平
void SDA_Set(IIC_type *IICx);                           // SDA输出高电平
bool IIC_read_SDA(IIC_type *IICx);                   //读SDA口状态
void IIC_start(IIC_type *IICx);                         //产生IIC起始信号
void IIC_stop(IIC_type *IICx) ;                         //产生IIC停止信号
bool IIC_wait_Ack(IIC_type *IICx);                   //等待应答信号到来
void IIC_Ack(IIC_type *IICx);                           //产生ACK应答 //SDA口应答需为低电平
void IIC_NAck(IIC_type *IICx);                          //不产生ACK应答 SDA为高
void IIC_Send_Byte(IIC_type *IICx, uint8_t byte_data);  //IIC发送一个字节
uint8_t IIC_Read_Byte(IIC_type *IICx, uint8_t ack);     //IIC读1个字节


#endif /* IIC_IO_H_ */
