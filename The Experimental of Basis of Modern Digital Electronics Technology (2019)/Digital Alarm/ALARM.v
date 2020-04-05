//hour, min, sec: 时，分，秒计数器输出信号，均采用BCD码计数器，然后组合后输入数码管显示
//mode：功能键按钮，按下以后三种模式循环----0：自然种；1：闹钟；2：校时
//turn: 选择改变时还是分
//chang: 时间模式下的时间调整按键，按一次加一
//上电之后先显示的是自然钟，m=0;


module ALARM (clk, clk_1k, mode, change, turn, alert, num_out, 
					LD_alert, LD_hour, LD_min);
	input clk, clk_1k, mode, change, turn;	//clk: 系统时钟，4HZ
														//clk_1k: 闹钟、整点报时音频信号，1KHZ
														//mode 模式选择
														//turn 选择调整小时还是分钟
														//change 按一次加一，长按快速加
		
	output alert, LD_alert, LD_hour, LD_min;//alert蜂鸣器输出
														 //LD_alert 连LED,显示是否设定闹钟
														 //LD_hour 是否在调整小时
														 //LD_min 是否在调整分钟
	output[23:0] num_out;						 
	
	reg[23:0] num_out;
	reg[7:0] hour, min, sec, hour1, min1, sec1, ahour, amin;
	reg[1:0] m, fm, num1, num2, num3, num4;
	reg[1:0] loop1, loop2, loop3, loop4, sound;
	reg LD_hour, LD_min;
	reg clk_1Hz, clk_2Hz, minclk, hclk;
	reg alert1, alert2, ear;
	reg count1, count2, counta, countb;
	wire ct1, ct2, cta, ctb, m_clk, h_clk;
	
	always @(posedge clk)	//控制整点报时的秒脉冲
		begin clk_2Hz <= ~clk_2Hz;
			if(sound == 3)	begin sound <= 0; ear <= 1; end
			else begin sound <= sound+1; ear <= 0; end
		end
	
	always @(posedge clk_2Hz)	//产生1Hz信号
		clk_1Hz <= ~clk_1Hz;
	
	always @(posedge mode)
		begin if(m == 2) m <=0;
					else	m <= m+1;
		end
		
	always @(posedge turn)
		fm <= ~fm;
	always 
		begin case(m)
				2:	begin 
					if(fm)
						begin count1 <= ~change; {LD_min, LD_hour} <= 2; end
						else begin counta <= ~change; {LD_min, LD_hour} <= 1;end
						{count2, countb} <= 0;
					end
				1: begin
					if(fm)
						begin count2 <= ~change; {LD_min, LD_hour} <= 2; end
						else begin countb <= ~change; {LD_min, LD_hour} <= 1; end
						{count1, counta} <= 2'b00;
					end
				default: {count1, count2, counta, countb, LD_min, LD_hour} <= 0;
				endcase
		end
		
		always @(negedge clk) //闹钟模式下调整分的时候change键有没有长按，有则num1 = 1，加速定时;
			if(count2) begin
				if(loop1 == 3) num1 <= 1;
				else begin loop1 <= loop1+1; num1 <= 0; end end
			else begin loop1 <= 0; num1 <= 0; end
		
		always @(negedge clk) //闹钟模式下调整时的时候change键有没有长按，有则num2 = 1，加速定时;
			if(countb) begin
				if(loop2 == 3) num2 <= 1;
				else begin loop2 <= loop2+1; num2 <= 0; end end
			else begin loop2 <= 0; num2 <= 0; end

		always @(negedge clk) //校时模式下调整分的时候change键有没有长按，有则num3 = 1，加速定时;
			if(count1) begin
				if(loop3 == 3) num3 <= 1;
				else begin loop3 <= loop3+1; num3 <= 0; end end
			else begin loop3 <= 0; num3 <= 0; end

		always @(negedge clk) //校时模式下调整时的时候change键有没有长按，有则num4 = 1，加速定时;
			if(counta) begin
				if(loop4 == 3) num4 <= 1;
				else begin loop4 <= loop4+1; num4 <= 0; end end
			else begin loop4 <= 0; num4 <= 0; end
			
		assign ct1 = (num3 & clk)|(!num3 & m_clk);
																//ct1: 计时、校时的分钟数计数器时钟
																//m_clk: 没有长按change的情况下的分钟进位信号（自然、慢调）
		assign cta = (num4 & clk)|(!num4 & h_clk);
																//cta: 计时、校时的小时数计数器时钟
		assign ct2 = (num1 & clk)|(!num1 & count2);
																//ct2: 闹钟模式的分钟数计数器时钟
		assign ctb = (num2 & clk)|(!num2 & countb);
																//ctb: 闹钟模式的分钟数计数器时钟
		
		always @(posedge clk_1Hz)	//秒计时和秒校时
											//若按下turn键并且M在M=0(时钟)下则秒清零
			if(!(sec1^8'h59) | !turn&(!m))
				begin sec1 <= 0; if(!(!turn&(!m)))	minclk <= 1; end //minclk分钟进位信号
			else begin
				if(sec1[3:0] == 4'b1001)
					begin sec1[3:0] <= 4'b0000; sec1[7:4] <= sec1[7:4]+1; end//sec低位为9的时候，清零后高位加1
					else sec1[3:0] <= sec1[3:0]+1;//sec低位不为9的时候，sec+1
				minclk <= 0; end
				
		assign m_clk = minclk || count1;
		
		always @(posedge ct1)	//分计时和分校时
			if(min1 == 8'h59)
				begin min1 <= 0; hclk <= 1; end
			else begin
				if(min1[3:0] == 9)
					begin min1[3:0] <= 0; min1[7:4] <= min1[7:4]+1; end
				else min1[3:0] <= min1[3:0]+1;
				hclk <= 0; end
		
		assign h_clk = hclk || counta;
		
		always @(posedge cta)	//校时分
			if(hour1 == 8'h23)	hour1 <= 0;
			else	if(hour1[3:0] == 9)
						begin	hour1[7:4] <= hour1[7:4]+1; hour1[3:0] <= 0; end
					else hour1[3:0] <= hour1[3:0]+1;
		
		always @(posedge ct2)	//闹钟模式下，分定时
			if(amin == 8'h59)	amin <= 0;
			else if(amin[3:0] == 9)
						begin amin[3:0] <= 0; amin[7:4] <= amin[7:4]+1; end
					else amin[3:0] <= amin[3:0]+1;
		
		always @(posedge ctb)	//闹钟模式下，小时定时
			if(ahour == 8'h23)	ahour <=0;
			else if(ahour[3:0] == 9)
						begin ahour[3:0] <= 0; ahour[7:4] <= ahour[7:4]+1; end
					else ahour[3:0] <= ahour[3:0]+1;
		
		always@(posedge clk)begin	
			if((min1 == amin)&&(hour1 == ahour)&&(amin|ahour)&&(change))
				if(sec1 < 8'h20)	alert1 <= 1;
				else alert1 <= 0;
			else alert1 <= 0;
		end
			
		always
			case(m)
			3'b00: begin hour <= hour1; min <= min1; sec <= sec1; end
			3'b01: begin hour <= ahour; min <= amin; sec <= 8'hzz; end
			3'b10: begin hour <= hour1; min <= min1; sec <= 8'hzz; end
			endcase
		
		always@(posedge clk)begin
			num_out[23:0] <= {hour[7:0], min[7:0], sec[7:0]};	//组合数码管输出
		end
		
		assign LD_alert = (ahour|amin)?1:0;
		assign alert = ((alert1)?clk_1k&clk:0)|alert2;//产生整点报时
		always
			if((min1 == 8'h59)&&(sec1 > 8'h54)||(!(min1|sec1)))
				if(sec1 > 8'h54)	alert2 <= ear&clk_1k;
				else	alert2 <= !ear&clk_1k;
			else	alert2 <= 0;
		
		endmodule
			
		