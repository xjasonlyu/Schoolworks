module div_1kHz(clk_out,
                clk,
                rst_n);
    
    input clk, rst_n;
    output clk_out;
    
    reg clk_out;
    reg [31:0] cnt;
    
    always @ (posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnt     <= 5'b0;
            clk_out <= 1'b0;
        end
        else if (cnt < 32'd24999) begin
            cnt     <= cnt + 1'b1;
            clk_out <= clk_out;
        end
        else begin
            cnt     <= 5'b0;
            clk_out <= ~clk_out;
        end
    end
    
endmodule
