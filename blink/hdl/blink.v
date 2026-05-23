module blink (
    input   CLK,
    input   RST,
    output reg [2:0]   LED_RGB
);

// 1. 初期値を明示的に指定 (reg [25:0] cnt26 = 0;)
reg [25:0] cnt26 = 0;

always @( posedge CLK ) begin
    cnt26 <= cnt26 + 1'h1;
end


wire ledcnten = (cnt26==26'h3ffffff); 

reg [2:0] cnt3 = 0; // ここも初期値を指定

always @(posedge CLK) begin
    if (ledcnten)
        if (cnt3 >= 3'd4)
            cnt3 <= 3'h0;
        else
            cnt3 <= cnt3 + 3'h1;
end

/* LED decoder */

always @* begin
    case (cnt3)
        3'd0:    LED_RGB = 3'b100; // 赤
        3'd1:    LED_RGB = 3'b010; // 緑
        3'd2:    LED_RGB = 3'b001; // 青
        3'd3:    LED_RGB = 3'b111; // 白
        3'd4:    LED_RGB = 3'b000; // 消灯
        default: LED_RGB = 3'b000;
    endcase
end


/* テスト用：クロックを直接LEDに出力する */
/*always @* begin
    LED_RGB[0] = CLK;   // 青色のLEDが高速（125MHz）で点滅＝人間には薄暗く点灯して見える
    LED_RGB[1] = 1'b0;
    LED_RGB[2] = 1'b0;
end
*/
endmodule

