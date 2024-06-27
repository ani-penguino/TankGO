/*
 * Avalon memory-mapped peripheral that generates VGA
 *
 * Stephen A. Edwards
 * Columbia University
 */ 

module vga_ball(input logic        clk,
	        input logic 	   reset,
		input logic [7:0]  writedata,
		input logic 	   write,
		input 		   chipselect,
		input logic [4:0]  address,

		output logic [7:0] VGA_R, VGA_G, VGA_B,
		output logic 	   VGA_CLK, VGA_HS, VGA_VS,
		                   VGA_BLANK_n,
		output logic 	   VGA_SYNC_n);

   logic [10:0]	   hcount;
   logic [9:0]     vcount;

   logic [7:0] 	   map, score, map_end_en, tank1_x, tank1_y, tank2_x, tank2_y, tank1_dir, tank2_dir, bullet1_x, bullet1_y, bullet2_x, bullet2_y, bullet1_dir, bullet2_dir, explod_x, explod_y;

   logic [999:0] map1 = 1000'b1111111111111111111111111111111111111111100000000000000000000000000000000000000110000000000000000000000000000000000000011000000000000000000000000000000000000001100011111000001111111111110000011111000110001000000000000001100000000000000100011000100000000000000110000000000000010001100010000000000000011000000000000001000110001000010000000001100000000010000100011000000001000000000110000000001000000001100000000100000000000000000000100000000110000000010000000000000000000010000000011000000001111111000000000111111000000001100000000100000000000000000000100000000110000000010000000000000000000010000000011000000001000000000110000000001000000001100010000100000000011000000000100001000110001000000000000001100000000000000100011000100000000000000110000000000000010001100010000000000000011000000000000001000110001111100000111111111111000001111100011000000000000000000000000000000000000001100000000000000000000000000000000000000110000000000000000000000000000000000000011111111111111111111111111111111111111111;
   logic [999:0] map2 = 1000'b1111111111111111111111111111111111111111100000000000000000000000000000000000000110000000000000000000000000000000000000011000000001111000011111100001111000000001100011111111111111111111111111111111000110000000000001111000000111100000000000011000000000000000000000000000000000000001100000000000000000000000000000000000000110001111111000001111111100000111111100011000000000100000000000000000010000000001100000000011110000000000001111000000000110000111001000001110011100000100111000011000010100100000111001110000010010100001100001000011110011000011001111000010000110000101001000001110011100000100101000011000011100100000111001110000010011100001100000000011110011111111001111000000000110000000001000000000000000000100000000011000010100100000000000000000010010100001100001010011111111000011111111001010000110000111000000000000000000000000111000011000000000000000000110000000000000000001100000000000000000011000000000000000000110000000000000111111111111000000000000011111111111111111111111111111111111111111;
   logic [999:0] map3 = 1000'b0000000000000000000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111000000001000000001000000001000000001001100000000100000000100000000100000000100110011111110000011110011110010011111110011000000000001001000000001001000000000001100000000000100100000000100100000000000111111111110010010011110010010010010011111000001001001001001000001001001001001001100000100100100100100000100100100100100110000010010010010011111110011110010000011000000000001000000000000001000001000001100011111100111111100100100100100100000110000000010000000000010010010010010000011000000001000000000001001001001001000001100111100111100111100100111100100111111110010000000000010010010000000010000000011001000000000001001001000000001000000001100100111100100100100111111111111111100110000000010010000010000010000000000010011111111111001111001111001111001000111001100000000000000000000100000000100000000110000000000000000000010000000010000000011111111111111111111111111111111111111111;
    logic [383:0] logo = 384'b000000000000000000000000000000000000000000000000000110001110001111100100010100001010000010010000000000010001001000010010010110001001000100010000000110010001001110010001010101001001000100010000000110010001000000010000110101001001111100010000000110010001001000010001010100101000101000010000000110001110000111100010010100011000010001111100000000000000000000000000000000000000000000000000;
    logic [479:0] game_over = 480'b000000000000000000000000000000000000000000000000000000000000001100110111110000100000111100011111010000010110001100111100000110110000110001110001100110000011010010010110001100100010000011110011110011011001100110001111011111110111111100110010000100110000110110001101100110000011011111110110001100000010000100110000110110001101100110000011011101110011011000000010000111110111110110001100111100011111011000110001110000111100000000000000000000000000000000000000000000000000000000000000;
    logic [127:0] hp = 128'b00000000000000000110000110110011011000011011001100001111101111110000100110111111011010011011001101101111101100110000000000000000;


    logic[2:0] counter = 0;
    logic[2:0] counter_y = 0;
    logic[10:0] real_x = 0;
    logic[10:0] real_y = 0;
    logic[10:0] real_tank_x1;
    logic[10:0] real_tank_x2;
    logic[10:0] real_tank_y1;
    logic[10:0] real_tank_y2;
    logic[10:0] real_bullet_x1;
    logic[10:0] real_bullet_x2;
    logic[10:0] real_bullet_y1;
    logic[10:0] real_bullet_y2;
    logic[10:0] res;
	  logic[10:0] res2;
    logic[10:0] minus_tmp1;
    logic[10:0] minus_tmp2;
    logic[10:0] minus_tmp3;
    logic[10:0] minus_tmp4;
    logic[10:0] cur;
    logic[10:0] cur_logo;
    logic[10:0] cur_game_over;
    logic[10:0] addr;
    logic[10:0] cur_hp_l;
    logic[10:0] cur_hp_r;
    logic[23:0] q;
    logic[20:0] tmp1;
    logic[20:0] tmp2;
    logic[20:0] tmp3;
    logic[20:0] tmp4;
    logic[30:0] result1;
    logic[30:0] result2;

    pic_rom pic_rom_inst(
      .address(addr),
      .clock(clk),
      .q(q)
    );

    parameter	RED  = 24'b00000000_01001000_01110111;
    parameter	BLUE = 24'b01111100_00000000_00000000;				
    parameter	WHITE = 24'b11111111_11111111_11111111;
    parameter   MAP_COLOR = 24'b10000100_10100100_11000100;
    parameter   DARK_BROWN = 24'b00110011_01000000_01011100;
    parameter	GREEN = 24'b00000000_11111111_00000000;
    parameter	BLACK = 24'b00000000_00000000_00000000;

    vga_counters counters(.clk50(clk), .*);

    assign res2 = 8'd16 * (16 - score[3:0]);
	  assign res = 8'd16 * (16 - score[7:4]);
    assign real_tank_x1 = tank1_x * 4;
    assign real_tank_x2 = tank2_x * 4;
    assign real_tank_y1 = tank1_y * 4;
    assign real_tank_y2 = tank2_y * 4;
    assign real_bullet_x1 = bullet1_x * 4;
    assign real_bullet_x2 = bullet2_x * 4;
    assign real_bullet_y1 = bullet1_y * 4;
    assign real_bullet_y2 = bullet2_y * 4;
    assign cur_logo = (vcount >> 2) * 8'd48 + ((hcount[10:1] - 8'd208) >> 2);
    assign cur_game_over = ((vcount - 8'd224) >> 2) * 8'd60 + ((hcount[10:1] - 8'd200) >> 2);
    assign cur_hp_l = (vcount >> 2) * 8'd16 + ((hcount[10:1] - 8'd64) >> 2);
    assign cur_hp_r = (vcount >> 2) * 8'd16 + ((hcount[10:1] - 8'd512) >> 2);
    assign cur = ((vcount - 8'd80) >> 4) * 8'd40 + (hcount[10:1] >> 4);
    assign minus_tmp1 = (hcount[10:1] > real_bullet_x1) ? hcount[10:1] - real_bullet_x1 : real_bullet_x1 - hcount[10:1];
    assign minus_tmp2 = (vcount > real_bullet_y1) ? vcount - real_bullet_y1 :  real_bullet_y1 - vcount;
    assign minus_tmp3 = (hcount[10:1] > real_bullet_x2) ? hcount[10:1] - real_bullet_x2 :  real_bullet_x2 - hcount[10:1];
    assign minus_tmp4 = (vcount > real_bullet_y2) ? vcount - real_bullet_y2 : real_bullet_y2 - vcount;
    assign tmp1 = minus_tmp1 * minus_tmp1;
    assign tmp2 = minus_tmp2 * minus_tmp2;
    assign tmp3 = minus_tmp3 * minus_tmp3;
    assign tmp4 = minus_tmp4 * minus_tmp4;
    assign result1 = tmp1 + tmp2;
    assign result2 = tmp3 + tmp4;

   always_ff @(posedge clk)
     if (reset) begin
      map <= 8'h0;
      score <= 8'h0;
      map_end_en <= 8'h0;
      tank1_x <= 8'd40;
      tank1_y <= 8'd70;
      tank2_x <= 8'd80;
      tank2_y <= 8'd50;
      tank1_dir <= 8'd0;
      tank2_dir <= 8'd2;
      bullet1_x <= 8'd12;
      bullet1_y <= 8'd32;
      bullet2_x <= 8'd69;
      bullet2_y <= 8'd50;
      bullet1_dir <= 8'd0;
      bullet2_dir <= 8'd0;
      explod_x <= 8'd0;
      explod_y <= 8'd0;
     end else if (chipselect && write)
       case (address)
        5'd0 : map <= writedata;
        5'd1 : score <= writedata;
        5'd2 : map_end_en <= writedata;
        5'd3 : tank1_x <= writedata;
        5'd4 : tank1_y <= writedata;
        5'd5 : tank2_x <= writedata;
        5'd6 : tank2_y <= writedata;
        5'd7 : tank1_dir <= writedata;
        5'd8 : tank2_dir <= writedata;
        5'd9 : bullet1_x <= writedata;
        5'd10 : bullet1_y <= writedata;
        5'd11 : bullet2_x <= writedata;
        5'd12 : bullet2_y <= writedata;
        5'd13 : bullet1_dir <= writedata;
        5'd14 : bullet2_dir <= writedata;
        5'd15 : explod_x <= writedata;
        5'd16 : explod_y <= writedata;
       endcase

   always_ff @(posedge clk) begin
      if (reset)
        {VGA_R, VGA_G, VGA_B} <= {8'h0, 8'h0, 8'h0};
      else if (VGA_BLANK_n )begin 
        if(map_end_en[0] == 1'b1) begin
          if(vcount >= 224 && vcount < 256 && hcount[10:1] >=200 && hcount[10:1] < 440) begin
            if(game_over[cur_game_over] == 1'b1) begin
              {VGA_R, VGA_G, VGA_B} <= WHITE;
            end else
              {VGA_R, VGA_G, VGA_B} <= {8'h0, 8'h0, 8'h0};
          end else
  			    {VGA_R, VGA_G, VGA_B} <= {8'h0, 8'h0, 8'h0};
        end else begin
          if(vcount < 10'd80) begin
            if(vcount < 32) begin
              if(hcount[10:1] >= 208 && hcount[10:1] < 400) begin
                if(logo[cur_logo] == 1'b1) 
                  {VGA_B, VGA_G, VGA_R} <= WHITE;
                else 
                  {VGA_B, VGA_G, VGA_R} <= BLACK;
              end else if(hcount[10:1] >= 64 && hcount[10:1] < 128) begin
                if(hp[cur_hp_l] == 1'b1) begin
                  {VGA_B, VGA_G, VGA_R} <= WHITE;
                end else 
                  {VGA_B, VGA_G, VGA_R} <= BLACK;
              end else if(hcount[10:1] >= 512 && hcount[10:1] < 576) begin
                if(hp[cur_hp_r] == 1'b1) begin
                  {VGA_B, VGA_G, VGA_R} <= WHITE;
                end else 
                  {VGA_B, VGA_G, VGA_R} <= BLACK;
              end else
                {VGA_B, VGA_G, VGA_R} <= BLACK;
            end else if(vcount >= 32 && vcount < 48) begin
              if((hcount[10:1] >=0 && hcount[10:1] < res) || (hcount[10:1] >= 640 - res2)) begin
                {VGA_B, VGA_G, VGA_R} <= GREEN;
              end else begin
                {VGA_B, VGA_G, VGA_R} <= BLACK;
              end
            end else begin
              {VGA_B, VGA_G, VGA_R} <= BLACK;
            end
          end else begin
            if(hcount[10:1] >= real_tank_x1 && hcount[10:1] < real_tank_x1 + 16 && vcount >= real_tank_y1 && vcount < real_tank_y1+16) begin
              case(tank1_dir)
                8'b00000000: addr <= (10'd15 - (hcount[10:1] - real_tank_x1[9:0])) * 16 + (vcount - real_tank_y1[9:0]);           //000
                8'b00000111: addr <= 10'd256 + (10'd15 - (hcount[10:1] - real_tank_x1[9:0])) * 16 + (vcount - real_tank_y1[9:0]); //111
                8'b00000010: addr <= (vcount - real_tank_y1[9:0]) * 16 + (10'd15 - (hcount[10:1] - real_tank_x1[9:0]));           //010
                8'b00000011: addr <= 10'd256 + (vcount - real_tank_y1[9:0]) * 16 + (10'd15 - (hcount[10:1] - real_tank_x1[9:0])); //011 correct
                8'b00000100: addr <= (hcount[10:1] - real_tank_x1[9:0]) * 16 + (10'd15 - (vcount - real_tank_y1[9:0]));           //100
                8'b00000001: addr <= 10'd256 + (10'd15 - (vcount - real_tank_y1[9:0])) * 16 + (10'd15 - (hcount[10:1] - real_tank_x1[9:0])); //011
                8'b00000110: addr <= (vcount - real_tank_y1[9:0]) * 16 + (hcount[10:1] - real_tank_x1[9:0]);                     //110
                8'b00000101: addr <= 10'd256 + (vcount - real_tank_y1[9:0]) * 16 + (hcount[10:1] - real_tank_x1[9:0]);           //101 correct
                default: addr <= (vcount - real_tank_y1[9:0]) * 16 + (hcount[10:1] - real_tank_x1[9:0]);
              endcase
              {VGA_B, VGA_G, VGA_R} <= q;
            end else if(hcount[10:1] >= real_tank_x2 && hcount[10:1] < real_tank_x2 + 16 && vcount >= real_tank_y2 && vcount < real_tank_y2+16) begin 
              //addr <= 10'd512 + (vcount - real_tank_y2[9:0]) * 16 + (hcount[10:1] - real_tank_x2[9:0]);
              case(tank2_dir)
                8'b00000000: addr <= 10'd512 + (10'd15 - (hcount[10:1] - real_tank_x2[9:0])) * 16 + (vcount - real_tank_y2[9:0]);
                8'b00000111: addr <= 10'd768 + (10'd15 - (hcount[10:1] - real_tank_x2[9:0])) * 16 + (vcount - real_tank_y2[9:0]);
                8'b00000010: addr <= 10'd512 + (vcount - real_tank_y2[9:0]) * 16 + (10'd15 - (hcount[10:1] - real_tank_x2[9:0]));
                8'b00000011: addr <= 10'd768 + (vcount - real_tank_y2[9:0]) * 16 + (10'd15 - (hcount[10:1] - real_tank_x2[9:0]));
                8'b00000100: addr <= 10'd512 + (hcount[10:1] - real_tank_x2[9:0]) * 16 + (10'd15 - (vcount - real_tank_y2[9:0]));
                8'b00000001: addr <= 10'd768 + (10'd15 - (vcount - real_tank_y2[9:0])) * 16 + (10'd15 - (hcount[10:1] - real_tank_x2[9:0]));
                8'b00000110: addr <= 10'd512 + (vcount - real_tank_y2[9:0]) * 16 + (hcount[10:1] - real_tank_x2[9:0]);
                8'b00000101: addr <= 10'd768 + (vcount - real_tank_y2[9:0]) * 16 + (hcount[10:1] - real_tank_x2[9:0]);
                default: addr <= 10'd512 + (vcount - real_tank_y2[9:0]) * 16 + (hcount[10:1] - real_tank_x2[9:0]);
              endcase
              {VGA_B, VGA_G, VGA_R} <= q;
            end else if(result1 < 9'd16)begin 
              {VGA_B, VGA_G, VGA_R} <= GREEN;
            end else if(result2 < 9'd16) begin 
              {VGA_B, VGA_G, VGA_R} <= GREEN;
            end else begin
              case(map)
                8'd0: begin
                  if(map1[cur] == 1'b1)
                    {VGA_B, VGA_G, VGA_R} <= MAP_COLOR;
                  else
                    {VGA_B, VGA_G, VGA_R} <= BLACK;
                end
                8'd1: begin
                  if(map2[cur] == 1'b1)
                    {VGA_B, VGA_G, VGA_R} <= MAP_COLOR;
                  else
                    {VGA_B, VGA_G, VGA_R} <= BLACK;
                end
                8'd2: begin
                  if(map3[cur] == 1'b1)
                    {VGA_B, VGA_G, VGA_R} <= MAP_COLOR;
                  else
                    {VGA_B, VGA_G, VGA_R} <= BLACK;
                end
                default: begin
                  {VGA_B, VGA_G, VGA_R} <= BLACK;
                end
              endcase
            end
          end
        end
      end else
        {VGA_R, VGA_G, VGA_B} <= {8'h0, 8'h0, 8'h0};
   end
	       
endmodule

module vga_counters(
 input logic 	     clk50, reset,
 output logic [10:0] hcount,  // hcount[10:1] is pixel column
 output logic [9:0]  vcount,  // vcount[9:0] is pixel row
 output logic 	     VGA_CLK, VGA_HS, VGA_VS, VGA_BLANK_n, VGA_SYNC_n);

/*
 * 640 X 480 VGA timing for a 50 MHz clock: one pixel every other cycle
 * 
 * HCOUNT 1599 0             1279       1599 0
 *             _______________              ________
 * ___________|    Video      |____________|  Video
 * 
 * 
 * |SYNC| BP |<-- HACTIVE -->|FP|SYNC| BP |<-- HACTIVE
 *       _______________________      _____________
 * |____|       VGA_HS          |____|
 */
   // Parameters for hcount
   parameter HACTIVE      = 11'd 1280,
             HFRONT_PORCH = 11'd 32,
             HSYNC        = 11'd 192,
             HBACK_PORCH  = 11'd 96,   
             HTOTAL       = HACTIVE + HFRONT_PORCH + HSYNC +
                            HBACK_PORCH; // 1600
   
   // Parameters for vcount
   parameter VACTIVE      = 10'd 480,
             VFRONT_PORCH = 10'd 10,
             VSYNC        = 10'd 2,
             VBACK_PORCH  = 10'd 33,
             VTOTAL       = VACTIVE + VFRONT_PORCH + VSYNC +
                            VBACK_PORCH; // 525

   logic endOfLine;
   
   always_ff @(posedge clk50 or posedge reset)
     if (reset)          hcount <= 0;
     else if (endOfLine) hcount <= 0;
     else  	         hcount <= hcount + 11'd 1;

   assign endOfLine = hcount == HTOTAL - 1;
       
   logic endOfField;
   
   always_ff @(posedge clk50 or posedge reset)
     if (reset)          vcount <= 0;
     else if (endOfLine)
       if (endOfField)   vcount <= 0;
       else              vcount <= vcount + 10'd 1;

   assign endOfField = vcount == VTOTAL - 1;

   // Horizontal sync: from 0x520 to 0x5DF (0x57F)
   // 101 0010 0000 to 101 1101 1111
   assign VGA_HS = !( (hcount[10:8] == 3'b101) &
		      !(hcount[7:5] == 3'b111));
   assign VGA_VS = !( vcount[9:1] == (VACTIVE + VFRONT_PORCH) / 2);

   assign VGA_SYNC_n = 1'b0; // For putting sync on the green signal; unused
   
   // Horizontal active: 0 to 1279     Vertical active: 0 to 479
   // 101 0000 0000  1280	       01 1110 0000  480
   // 110 0011 1111  1599	       10 0000 1100  524
   assign VGA_BLANK_n = !( hcount[10] & (hcount[9] | hcount[8]) ) &
			!( vcount[9] | (vcount[8:5] == 4'b1111) );

   /* VGA_CLK is 25 MHz
    *             __    __    __
    * clk50    __|  |__|  |__|
    *        
    *             _____       __
    * hcount[0]__|     |_____|
    */
   assign VGA_CLK = hcount[0]; // 25 MHz clock: rising edge sensitive
   
endmodule






