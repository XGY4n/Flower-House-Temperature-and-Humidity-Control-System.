#include "select573.h"
void SelectHC573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;  //高三位清零：0001 1111 高三位1000 0000
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;  //高三位清零：0001 1111 高三位1010 0000
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;  //高三位清零：0001 1111 高三位1100 0000
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;  //高三位清零：0001 1111 高三位1110 0000
			break;
		case 0:
			P2=(P2&0x1f)|0x00;
		  break;
	}
}