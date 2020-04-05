module shumaguan(
   //input
   input                  clk    ,        // 时钟信号
   input                  rst_n  ,        // 复位信号
   input        [23:0]    num    ,        // 6个数码管要显示的数值
	
   //output
   output  reg  [5:0]     sel    ,        // 数码管位选
   output  reg  [7:0]     seg_led         // 数码管段选
);

//parameter define
parameter  WIDTH0 = 50_000;
localparam  point = 6'b010100;
//reg define
reg    [15:0]             cnt0;           // 1ms计数
reg    [2:0]              cnt;            // 切换显示数码管用
reg    [3:0]              num1;           // 送给要显示的数码管，要亮的灯
reg                       point1;         // 要显示的小数点


//计数1ms
always @(posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0)
        cnt0 <= 15'b0;
    else if(cnt0 < WIDTH0)
        cnt0 <= cnt0 + 1'b1;
    else
        cnt0 <= 15'b0;
end

//计数器，用来计数6个状态（因为有6个灯）
always @(posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0)
        cnt <= 3'b0;
    else if(cnt < 3'd6) begin
        if(cnt0 == WIDTH0)
            cnt <= cnt + 1'b1;
        else
            cnt <= cnt;
    end
    else
        cnt <= 3'b0;
end

//6个数码管轮流显示，完成刷新（从右到左）
always @(posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0) begin
        sel     <= 6'b000001;
        num1 <= 4'b0;
    end
    else begin
        case (cnt)
            3'd0:begin
                 sel    <= 6'b111110;
                 num1   <= num[3:0] ;
                 point1 <= point[0] ;
            end
            3'd1:begin
                 sel    <= 6'b111101;
                 num1   <= num[7:4] ;
                 point1 <= point[1] ;
            end
            3'd2:begin
                 sel    <= 6'b111011;
                 num1   <= num[11:8];
                 point1 <= point[2] ;
            end
            3'd3:begin
                 sel    <= 6'b110111 ;
                 num1   <= num[15:12];
                 point1 <= point[3]  ;
            end
            3'd4:begin
                 sel    <= 6'b101111 ;
                 num1   <= num[19:16];
                 point1 <= point[4]  ;
            end
            3'd5:begin
                 sel    <= 6'b011111 ;
                 num1   <= num[23:20];
                 point1 <= point[5]  ;
            end
            default: begin
                 sel    <= 6'b000000;
                 num1   <= 4'b0;
                 point1 <= 1'b1;
            end
        endcase
    end
end

//数码管显示数据
always @ (posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0)
        seg_led <= 7'b0;
    else begin
        case(num1)
            4'd0: seg_led <= {~point1,7'b1000000};
            4'd1: seg_led <= {~point1,7'b1111001};
            4'd2: seg_led <= {~point1,7'b0100100};
            4'd3: seg_led <= {~point1,7'b0110000};
            4'd4: seg_led <= {~point1,7'b0011001};
            4'd5: seg_led <= {~point1,7'b0010010};
            4'd6: seg_led <= {~point1,7'b0000010};
            4'd7: seg_led <= {~point1,7'b1111000};
            4'd8: seg_led <= {~point1,7'b0000000};
            4'd9: seg_led <= {~point1,7'b0010000};
            default: seg_led <= {point1,7'b1000000};
        endcase
    end
end
endmodule