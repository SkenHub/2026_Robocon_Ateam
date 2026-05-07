
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo.h"
#include "sken_library/include.h"

Encoder enc;
Encoder_data enc_data;

Motor mtr;

Gpio sw;
Gpio limit;

CanData can_data;
uint8_t can_enc[8];
uint8_t can_limit[8];

int count, a,b,c,d;

float mtr_pwm;

void encoder_interrupt(){
	enc.interrupt(&enc_data);
}

void can_kakunou(void){
	if(can_data.rx_stdid == 0x333){
		for(int i=0;i<8;++i){
			can_enc[i] = can_data.rx_data[i];
		}
	}
	if(can_data.rx_stdid == 0x111){
		for(int i=0;i<8;++i){
			can_limit[i] = can_data.rx_data[i];
		}
	}
}


int main(void){
	sken_system.init();
	mtr.init(Apin,B8,TIMER2,CH1);
	mtr.init(Bpin,B9,TIMER2,CH2);
	enc.init(A0,A1,TIMER5);
	sw.init(C13,INPUT_PULLUP);
	limit.init(C11,INPUT_PULLUP);
	sken_system.addTimerInterruptFunc(encoder_interrupt,0,1);
	sken_system.startCanCommunicate(B13, B12, CAN_2);
	enc.reset();

	sken_system.addCanRceiveInterruptFunc(CAN_2, &can_data);
	sken_system.addTimerInterruptFunc(can_kakunou, 3, 1);
	while(1){
		sken_system.canTransmit(CAN_2, 0x333, can_enc, 8, 0);
		count = enc.read() * 180/4060;
		a = b;
		if(!sw.read()) b = 1;
		else b = 0;
			if(a == false&&b == true)c++;

		if(c%2 == 1){
			if(count < 40){
				if(d == 0) d = 1;
			}
			else if(count >= 40){
				d = 2;
			}
			if(count > 100) mtr_pwm = 0;
		}
		if(c%2 == 0){
			if(d == 2) d = 3;
			}
		if(limit.read()){
				c = 0;
				d = 0;
				can_enc[0] = 0;
				enc.reset();
		}

		if(d == 0) mtr_pwm = 0;
		if(d == 1) mtr_pwm = -35;
		if(d == 2){
			can_enc[0] = 1;
		}
		if(d == 3){
			can_enc[0] = 0;
			mtr_pwm = 10;
		}
		mtr.write(mtr_pwm);

	}
}


/*void encoder_interrupt(){
	enc.interrupt(&enc_data);
}

void can_kakunou(void){
	if(can_data.rx_stdid == 0x333){
		for(int i=0;i<8;++i){
			can_enc[i] = can_data.rx_data[i];
		}
	}
}

int main(void){
	sken_system.init();
	mtr.init(Apin,B8,TIMER2,CH1);
	mtr.init(Bpin,B9,TIMER2,CH2);
	enc.init(A0,A1,TIMER2);
	sw.init(C13,INPUT_PULLUP);
	sken_system.addTimerInterruptFunc(encoder_interrupt,0,1);
	sken_system.startCanCommunicate(B13, B12, CAN_2);

	sken_system.addCanRceiveInterruptFunc(CAN_2, &can_data);
	sken_system.addTimerInterruptFunc(can_kakunou, 3, 1);
	while(1){
		sken_system.canTransmit(CAN_2, 0x333, can_enc, 8, 0);
		count = enc.read() * 180/4060;

		a = b;
		if(!sw.read()) b = 1;
		else b = 0;
			if(a == false&&b == true)c++;

		if(c%3 == 1){
			if(count < 230) mtr_pwm = 20;
			else mtr.write(0);
		}
		if(c%3 == 2) mtr_pwm = -10;
		if(c%3 == 0) mtr_pwm = 0;
		if(count > 200){
			can_enc[0] = 1;
		}
		else can_enc[0] = 0;

		if(count == 360) enc.reset();

		mtr.write(mtr_pwm);
	}
}*/
