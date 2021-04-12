module aes_encrypt(input logic clk, input logic rst_n,
               // slave (CPU-facing)
               output logic slave_waitrequest,
               input logic [3:0] slave_address,
               input logic slave_read, output logic [31:0] slave_readdata,
               input logic slave_write, input logic [31:0] slave_writedata);

    // use 1D array for key, cipher block, and sbox
    // all of the arrays have to be read in from memory
    logic [7:0] sbox [0:255];
    logic [7:0] key [0:175];
    logic [7:0] block [0:15];
    logic [8:0] rcon [0:3];
    logic [7:0] copy_block [0:15];
    logic [7:0] mult2_block [0:15];

    logic [31:0] block0, block1, block2, block3;
    logic [3:0] r_i;
    logic [1:0] k_i;
    logic done;

    enum {START, SBOX, KEXP0, KEXP1, XOR, ROUND, SUB, SHIFT, MIX0, MIX1, DONE} state;

    always @(posedge clk) begin
        if (~rst_n) begin
            state <= START;

            slave_waitrequest <= 1'b1;

            r_i <= 4'b0;
            k_i <= 2'b1;

            rcon[0] <= 9'b1;
            rcon[1] <= 9'b0;
            rcon[2] <= 9'b0;
            rcon[3] <= 9'b0;

            done <= 1'b0;
        end
        else if (slave_write && slave_address === 4'd4) begin
            state <= START;

            block[0]  = slave_writedata[7:0];
            block[4]  = slave_writedata[15:8];
            block[8]  = slave_writedata[23:16];
            block[12] = slave_writedata[31:24];

            slave_waitrequest <= 1'b1;

            r_i <= 4'b0;
            k_i <= 2'b1;

            rcon[0] <= 9'b1;
            rcon[1] <= 9'b0;
            rcon[2] <= 9'b0;
            rcon[3] <= 9'b0;

            done <= 1'b0;
        end
        else begin
            case(state)
                START: begin
                    slave_waitrequest <= 1'b0;

                    r_i <= 4'b0;
                    k_i <= 2'b1;

                    rcon[0] <= 9'b1;
                    rcon[1] <= 9'b0;
                    rcon[2] <= 9'b0;
                    rcon[3] <= 9'b0;

                    done <= 1'b0;

                    if (slave_write) begin
                        if (slave_address === 4'd0) begin
                            // first 32 bit of key
                            key[0]  = slave_writedata[7:0];
                            key[4]  = slave_writedata[15:8];
                            key[8]  = slave_writedata[23:16];
                            key[12] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd1) begin
                            // second 32 bit of key
                            key[1]  = slave_writedata[7:0];
                            key[5]  = slave_writedata[15:8];
                            key[9]  = slave_writedata[23:16];
                            key[13] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd2) begin
                            // third 32 bit of key
                            key[2]  = slave_writedata[7:0];
                            key[6]  = slave_writedata[15:8];
                            key[10]  = slave_writedata[23:16];
                            key[14] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd3) begin
                            // fourth 32 bit of key
                            key[3]  = slave_writedata[7:0];
                            key[7]  = slave_writedata[15:8];
                            key[11]  = slave_writedata[23:16];
                            key[15] = slave_writedata[31:24];
                        end

                        else if (slave_address === 4'd4) begin
                            // first 32 bit of plaintext
                            block[0]  = slave_writedata[7:0];
                            block[4]  = slave_writedata[15:8];
                            block[8]  = slave_writedata[23:16];
                            block[12] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd5) begin
                            // second 32 bit of plaintext
                            block[1]  = slave_writedata[7:0];
                            block[5]  = slave_writedata[15:8];
                            block[9]  = slave_writedata[23:16];
                            block[13] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd6) begin
                            // third 32 bit of plaintext
                            block[2]  = slave_writedata[7:0];
                            block[6]  = slave_writedata[15:8];
                            block[10]  = slave_writedata[23:16];
                            block[14] = slave_writedata[31:24];
                        end
                        else if (slave_address === 4'd7) begin
                            // fourth 32 bit of plaintext
                            block[3]  = slave_writedata[7:0];
                            block[7]  = slave_writedata[15:8];
                            block[11]  = slave_writedata[23:16];
                            block[15] = slave_writedata[31:24];
                        end

                        else if (slave_address === 4'd8) begin
                            state <= SBOX;
                            slave_waitrequest <= 1'b1;
                        end
                        else if (slave_address === 4'd9) begin
                            state <= XOR;
                            slave_waitrequest <= 1'b1;
                        end
                    end
                end  

                // state to initialize sbox
                SBOX: begin
                    state <= KEXP0;

                    sbox[0:15]    <= '{8'h63, 8'h7c, 8'h77, 8'h7b, 8'hf2, 8'h6b, 8'h6f, 8'hc5, 8'h30, 8'h01, 8'h67, 8'h2b, 8'hfe, 8'hd7, 8'hab, 8'h76};
                    sbox[16:31]   <= '{8'hca, 8'h82, 8'hc9, 8'h7d, 8'hfa, 8'h59, 8'h47, 8'hf0, 8'had, 8'hd4, 8'ha2, 8'haf, 8'h9c, 8'ha4, 8'h72, 8'hc0};
                    sbox[32:47]   <= '{8'hb7, 8'hfd, 8'h93, 8'h26, 8'h36, 8'h3f, 8'hf7, 8'hcc, 8'h34, 8'ha5, 8'he5, 8'hf1, 8'h71, 8'hd8, 8'h31, 8'h15};
                    sbox[48:63]   <= '{8'h04, 8'hc7, 8'h23, 8'hc3, 8'h18, 8'h96, 8'h05, 8'h9a, 8'h07, 8'h12, 8'h80, 8'he2, 8'heb, 8'h27, 8'hb2, 8'h75};
                    sbox[64:79]   <= '{8'h09, 8'h83, 8'h2c, 8'h1a, 8'h1b, 8'h6e, 8'h5a, 8'ha0, 8'h52, 8'h3b, 8'hd6, 8'hb3, 8'h29, 8'he3, 8'h2f, 8'h84};
                    sbox[80:95]   <= '{8'h53, 8'hd1, 8'h00, 8'hed, 8'h20, 8'hfc, 8'hb1, 8'h5b, 8'h6a, 8'hcb, 8'hbe, 8'h39, 8'h4a, 8'h4c, 8'h58, 8'hcf};
                    sbox[96:111]  <= '{8'hd0, 8'hef, 8'haa, 8'hfb, 8'h43, 8'h4d, 8'h33, 8'h85, 8'h45, 8'hf9, 8'h02, 8'h7f, 8'h50, 8'h3c, 8'h9f, 8'ha8};
                    sbox[112:127] <= '{8'h51, 8'ha3, 8'h40, 8'h8f, 8'h92, 8'h9d, 8'h38, 8'hf5, 8'hbc, 8'hb6, 8'hda, 8'h21, 8'h10, 8'hff, 8'hf3, 8'hd2};
                    sbox[128:143] <= '{8'hcd, 8'h0c, 8'h13, 8'hec, 8'h5f, 8'h97, 8'h44, 8'h17, 8'hc4, 8'ha7, 8'h7e, 8'h3d, 8'h64, 8'h5d, 8'h19, 8'h73};
                    sbox[144:159] <= '{8'h60, 8'h81, 8'h4f, 8'hdc, 8'h22, 8'h2a, 8'h90, 8'h88, 8'h46, 8'hee, 8'hb8, 8'h14, 8'hde, 8'h5e, 8'h0b, 8'hdb};
                    sbox[160:175] <= '{8'he0, 8'h32, 8'h3a, 8'h0a, 8'h49, 8'h06, 8'h24, 8'h5c, 8'hc2, 8'hd3, 8'hac, 8'h62, 8'h91, 8'h95, 8'he4, 8'h79};
                    sbox[176:191] <= '{8'he7, 8'hc8, 8'h37, 8'h6d, 8'h8d, 8'hd5, 8'h4e, 8'ha9, 8'h6c, 8'h56, 8'hf4, 8'hea, 8'h65, 8'h7a, 8'hae, 8'h08};
                    sbox[192:207] <= '{8'hba, 8'h78, 8'h25, 8'h2e, 8'h1c, 8'ha6, 8'hb4, 8'hc6, 8'he8, 8'hdd, 8'h74, 8'h1f, 8'h4b, 8'hbd, 8'h8b, 8'h8a};
                    sbox[208:223] <= '{8'h70, 8'h3e, 8'hb5, 8'h66, 8'h48, 8'h03, 8'hf6, 8'h0e, 8'h61, 8'h35, 8'h57, 8'hb9, 8'h86, 8'hc1, 8'h1d, 8'h9e};
                    sbox[224:239] <= '{8'he1, 8'hf8, 8'h98, 8'h11, 8'h69, 8'hd9, 8'h8e, 8'h94, 8'h9b, 8'h1e, 8'h87, 8'he9, 8'hce, 8'h55, 8'h28, 8'hdf};
                    sbox[240:255] <= '{8'h8c, 8'ha1, 8'h89, 8'h0d, 8'hbf, 8'he6, 8'h42, 8'h68, 8'h41, 8'h99, 8'h2d, 8'h0f, 8'hb0, 8'h54, 8'hbb, 8'h16};
                end

                KEXP0: begin
                    if (r_i === 4'd10) begin
                        state <= XOR;
                        r_i <= 4'b0;
                    end
                    else begin
                        key[((r_i+1)<<4)+0] <= sbox[key[(r_i<<4)+7]] ^ key[(r_i<<4)+0] ^ rcon[0];
                        key[((r_i+1)<<4)+4] <= sbox[key[(r_i<<4)+11]] ^ key[(r_i<<4)+4] ^ rcon[1];
                        key[((r_i+1)<<4)+8] <= sbox[key[(r_i<<4)+15]] ^ key[(r_i<<4)+8] ^ rcon[2];
                        key[((r_i+1)<<4)+12] <= sbox[key[(r_i<<4)+3]] ^ key[(r_i<<4)+12] ^ rcon[3];
                        state <= KEXP1;
                    end
                end

                KEXP1: begin
                    key[((r_i+1)<<4)+k_i+0] <= key[(r_i<<4)+k_i+0] ^ key[((r_i+1)<<4)+k_i-1];
                    key[((r_i+1)<<4)+k_i+4] <= key[(r_i<<4)+k_i+4] ^ key[((r_i+1)<<4)+k_i+3];
                    key[((r_i+1)<<4)+k_i+8] <= key[(r_i<<4)+k_i+8] ^ key[((r_i+1)<<4)+k_i+7];
                    key[((r_i+1)<<4)+k_i+12] <= key[(r_i<<4)+k_i+12] ^ key[((r_i+1)<<4)+k_i+11];

                    k_i <= k_i + 2'b1;
                    if (k_i === 2'd3) begin
                        state <= KEXP0;
                        r_i <= r_i + 4'b1;
                        k_i <= 2'b1;

                        if (rcon[0] < 9'd80) begin
                            rcon[0] <= rcon[0] << 9'd1;
                        end
                        else begin
                            rcon[0] <= (rcon[0] << 9'd1) ^ 9'h11b;
                        end
                    end
                end

                ROUND: begin
                    if (r_i === 4'd10) begin
                        state <= DONE;
                        done <= 1'b1;

                        block0 <= (block[12] << 24) + (block[8] << 16) + (block[4] << 8) + (block[0]);
                        block1 <= (block[13] << 24) + (block[9] << 16) + (block[5] << 8) + (block[1]);
                        block2 <= (block[14] << 24) + (block[10] << 16) + (block[6] << 8) + (block[2]);
                        block3 <= (block[15] << 24) + (block[11] << 16) + (block[7] << 8) + (block[3]);
                    end
                    else begin
                        state <= SUB;
                        r_i <= r_i + 4'b1;
                    end
                end

                SUB: begin
                    // hopefully synthesizable for loop
                    for (int i = 0; i < 16; i++) begin
                        block[i] <= sbox[block[i]];
                    end
                    state <= SHIFT;
                end

                SHIFT: begin
                    block[4'd4] <= block[4'd5];
                    block[4'd5] <= block[4'd6];
                    block[4'd6] <= block[4'd7];
                    block[4'd7] <= block[4'd4];
                    block[4'd8] <= block[4'd10];
                    block[4'd9] <= block[4'd11];
                    block[4'd10] <= block[4'd8];
                    block[4'd11] <= block[4'd9];
                    block[4'd12] <= block[4'd15];
                    block[4'd13] <= block[4'd12];
                    block[4'd14] <= block[4'd13];
                    block[4'd15] <= block[4'd14];
                    state <= MIX0;
                end

                MIX0: begin
                    if (r_i === 4'd10) begin
                        state <= XOR;
                    end
                    else begin
                        for (int i = 0; i < 16; i++) begin
                            copy_block[i] <= block[i];
                            if (block[i][7] === 1'b1) begin
                                mult2_block[i] <= (block[i] << 8'b1) ^ 8'h1b;
                            end
                            else begin
                                mult2_block[i] <= block[i] << 8'b1;
                            end
                        end
                        state <= MIX1;
                    end
                end

                MIX1: begin
                    for (int i = 0; i < 4; i++) begin
                        block[i] <= mult2_block[i] ^ mult2_block[4+i] ^ copy_block[4+i] ^ copy_block[8+i] ^ copy_block[12+i];
                        block[4+i] <= mult2_block[4+i] ^ mult2_block[8+i] ^ copy_block[8+i] ^ copy_block[12+i] ^ copy_block[i];
                        block[8+i] <= mult2_block[8+i] ^ mult2_block[12+i] ^ copy_block[12+i] ^ copy_block[i] ^ copy_block[4+i];
                        block[12+i] <= mult2_block[12+i] ^ mult2_block[i] ^ copy_block[i] ^ copy_block[4+i] ^ copy_block[8+i];
                    end
                    state <= XOR;
                end

                XOR: begin
                    for (int i = 0; i < 16; i++) begin
                        block[i] <= block[i] ^ key[i + (r_i << 4) + 0];
                    end
                    state <= ROUND;
                end

                default: begin
                    state <= START;
                    slave_waitrequest <= 1'b1;
                    done <= 1'b0;
                end
            endcase
        end
    end

    // Reading
    always @(*) begin
        slave_readdata <= 0;
        if (slave_read) begin
            if (slave_address === 4'd0) begin
                slave_readdata <= block0;
            end
            else if (slave_address === 4'd1) begin
                slave_readdata <= block1;
            end
            else if (slave_address === 4'd2) begin
                slave_readdata <= block2;
            end
            else if (slave_address === 4'd3) begin
                slave_readdata <= block3;
            end
        end
    end

endmodule: aes_encrypt
