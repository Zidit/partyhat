#ifndef INCLUDE_BUTTON777
#define INCLUDE_BUTTON


#include <avr/io.h>


class button 
{
public:
	button(PORT_t* port, uint8_t pin_nbr)
	{ 
		this->port = port; 
		this->pin_mask = ( 1 << pin_nbr); 
		port->DIRCLR = pin_mask;
		volatile register8_t *ctrl_reg = &(port->PIN0CTRL) + pin_nbr;
		*ctrl_reg = PORT_OPC_PULLUP_gc; 		
		prev_state = port->IN & pin_mask;
	}
	
	bool isUp()
	{
		if (port->IN & pin_mask) return true;
		else return false;
	}

	bool isDown()
	{
		if (port->IN & pin_mask) return false;
		else return true;
	}

	bool isToggled()
	{
		uint8_t current_state = port->IN & pin_mask;
		bool return_value;
	
		if(current_state == prev_state) return_value = false;
		else return_value = true;

		prev_state = current_state;
		return return_value;
	}



private:

	PORT_t* port;
	uint8_t pin_mask;
	uint8_t prev_state;

};













#endif //INCLUDE_BUTTON

