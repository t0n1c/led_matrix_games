
#include <StandardCplusplus.h> //this lib is just to enable syntax highlighting on bitbucket

int interval,stack_width,head_y,head_x,previous_x,diff;
uint16_t current_color;
boolean flag;

void stack_setup()
{

	interval = 800/W - (difficulty * 400/W);
	stack_width = 4;
	head_y = H - 2;
	previous_x = (W - stack_width) / 2;
	matrix -> fillRect(previous_x * unit_pieces , (H - 1) * unit_pieces,stack_width * unit_pieces, unit_pieces,select_random_color());
	flag = false;
	current_color = select_random_color();

}


void stack_loop()
{
	

	for (head_x = 0; head_x + stack_width  < W; head_x++){
		matrix -> fillRect(head_x * unit_pieces,head_y * unit_pieces,stack_width * unit_pieces,unit_pieces,current_color);
		if (digitalRead(p1_joystick_button) == LOW){
			delay(140);
			update_stack();
			flag = true;
			break;
		}
		delay(interval);
		matrix -> fillRect(head_x * unit_pieces,head_y * unit_pieces,unit_pieces,unit_pieces,0);

	}

	if(!flag){
		for (head_x; head_x > 0; head_x--){
			matrix -> fillRect(head_x * unit_pieces,head_y * unit_pieces,stack_width * unit_pieces,unit_pieces,current_color);	
			if (digitalRead(p1_joystick_button) == LOW){
				delay(140);
				update_stack();
				break;
			}
			delay(interval);
			matrix -> fillRect((head_x + stack_width - 1) * unit_pieces,head_y * unit_pieces,unit_pieces,unit_pieces,0);
		}
	} else {
		flag = false;
	}
}


void update_stack()
{
	diff = abs(previous_x - head_x);
	/*Serial.println(head_x);
	Serial.println(previous_x);
	Serial.println(head_y);
	Serial.println(diff);
	Serial.println(stack_width);*/
	if (diff >= stack_width) {
		blink_rect(head_x,250,stack_width);
		game_over();
		while(1){}
	}
	stack_width -= diff;

	if (head_x < previous_x){
		blink_rect(head_x,40,diff);
		previous_x = head_x + diff;
	}else if (head_x > previous_x){
		blink_rect(head_x + stack_width,40,diff);
		previous_x = head_x;
	}
	head_y--;
	if (head_y < 0){
		matrix -> fillScreen(0);
		head_y = H - 2;
		previous_x = (W - stack_width) / 2;
		matrix -> fillRect(previous_x * unit_pieces , (H - 1) * unit_pieces,stack_width * unit_pieces, unit_pieces,select_random_color());
	}
	current_color = select_random_color();


}


void blink_rect(int x,int ms,int width_diff)
{
	int iter = 4;
	while (iter > 0){
		if (iter % 2 == 0){
			matrix -> fillRect(x * unit_pieces,head_y * unit_pieces,width_diff * unit_pieces,unit_pieces,0);
		} else {
			matrix -> fillRect(x * unit_pieces,head_y * unit_pieces,width_diff * unit_pieces,unit_pieces,current_color);
		}
		delay(ms);
		iter--;
	}
	matrix -> fillRect(x * unit_pieces,head_y * unit_pieces,width_diff * unit_pieces,unit_pieces,0);
}







