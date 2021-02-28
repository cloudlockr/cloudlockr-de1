// top level file
// Need to face 2 memories
// One to copy in data
// One to copy out data
// Alternatively, same memory, but HOST (CPU) determines which part of memory to copy in and copy out to

module aes_decrypt(input logic clk, input logic rst_n,
               // slave (CPU-facing)
               output logic slave_waitrequest,
               input logic [3:0] slave_address,
               input logic slave_read, output logic [31:0] slave_readdata,
               input logic slave_write, input logic [31:0] slave_writedata,
               // master source (SDRAM-facing)
               input logic master_waitrequest,
               output logic [31:0] master_address,
               output logic master_read, input logic [7:0] master_readdata, input logic master_readdatavalid,
               output logic master_write, output logic [7:0] master_writedata,
               // master dest (SDRAM-facing)
               input logic master2_waitrequest,
               output logic [31:0] master2_address,
               output logic master2_read, input logic [7:0] master2_readdata, input logic master2_readdatavalid,
               output logic master2_write, output logic [7:0] master2_writedata);

    // use 1D array for key, cipher block, and sbox
    // all of the arrays have to be read in from memory
    logic [7:0] sbox [0:255];
    logic [7:0] key [0:175];
    logic [7:0] block [0:15];
    logic [8:0] rcon [0:3];
    logic [7:0] copy_block [0:15];
    logic [7:0] mult2_block [0:15];

    logic [31:0] word0, word1, word2, word3;
    logic [27:0] n_blocks, block_num;
    logic [31:0] mem_i;
    logic [3:0] r_i, block_i;
    logic [1:0] k_i;
    logic done;

    enum {START, MEM0, MEM1, MEM2, MEM3, KEXP0, KEXP1, XOR, ROUND, SUB, SHIFT, MIX0, MIX1, DONE} state;

    always @(posedge clk, negedge rst_n) begin
        if (~rst_n) begin
            state <= START;

            slave_waitrequest <= 1'b1;
            master_read <= 1'b0;
            master_write <= 1'b0;
            master2_read <= 1'b0;
            master2_write <= 1'b0;
            n_blocks <= 28'b0;
            block_num <= 28'b0;

            mem_i <= 32'b0;
            block_i <= 4'b0;
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
                    master_read <= 1'b0;
                    master_write <= 1'b0;
                    master2_read <= 1'b0;
                    master2_write <= 1'b0;
                    block_num <= 28'b0;

                    mem_i <= 32'b0;
                    block_i <= 4'b0;
                    r_i <= 4'b0;
                    k_i <= 2'b1;

                    rcon[0] <= 9'b1;
                    rcon[1] <= 9'b0;
                    rcon[2] <= 9'b0;
                    rcon[3] <= 9'b0;

                    done <= 1'b0;

                    if (slave_read) begin
                        if (slave_address === 4'd0) begin
                            slave_readdata <= word0;
                        end
                        else if (slave_address === 4'd1) begin
                            slave_readdata <= word1;
                        end
                        else if (slave_address === 4'd2) begin
                            slave_readdata <= word2;
                        end
                        else if (slave_address === 4'd3) begin
                            slave_readdata <= word3;
                        end
                    end
                    else if (slave_write) begin
                        if (slave_address === 4'd0) begin
                            word0 <= slave_writedata;

                            if (n_blocks !== 28'b0) begin
                                state <= MEM0;
                                slave_waitrequest <= 1'b1;

                                master_address <= word2;
                                master_read <= 1'b1;
                            end
                        end
                        else if (slave_address === 4'd1) begin
                            // dest byte address
                            word1 <= slave_writedata;
                        end
                        else if (slave_address === 4'd2) begin
                            // src byte address
                            word2 <= slave_writedata;
                        end
                        else if (slave_address === 4'd3) begin
                            // number of bytes to read from src
                            word3 <= slave_writedata;
                            n_blocks <= slave_writedata >> 4;
                        end
                    end
                end  

                // states to copy in content from memory to sbox, key, and cipher block
                MEM0: begin
                    if (~master_waitrequest) begin
                        master_read <= 1'b0;
                    end
                    if (master_readdatavalid) begin
                        state <= MEM1;

                        if (mem_i <= 32'd255) begin
                            sbox[mem_i] <= master_readdata;
                        end
                        else if (mem_i <= 32'd271) begin
                            key[(block_i>>4'd2)+(block_i<<4'd2)] <= master_readdata;
                            block_i <= block_i + 4'b1;
                        end
                        else begin
                            block[(block_i>>4'd2)+(block_i<<4'd2)] <= master_readdata;
                            block_i <= block_i + 4'b1;
                        end
                    end
                end

                // states to copy in content from memory to sbox, key, and cipher block
                MEM1: begin
                    if (mem_i === 32'd287) begin
                        state <= KEXP0;
                        mem_i <= mem_i + 32'b1;
                    end
                    else begin
                        state <= MEM0;

                        mem_i <= mem_i + 32'b1;
                        master_read <= 1'b1;
                        master_address <= master_address + 32'b1;
                    end
                end

                // state to copy cipher block to dest memory and copy in new block
                MEM2: begin
                    if (~master_waitrequest) begin
                        master_read <= 1'b0;
                    end
                    if (~master2_waitrequest) begin
                        master2_write <= 1'b0;
                    end
                    if (master_readdatavalid) begin
                        state <= MEM3;
                        block[(block_i>>4'd2)+(block_i<<4'd2)] = master_readdata;

                        block_i <= block_i + 4'b1;
                        if (block_i + 4'b1 === 4'b0) begin
                            block_num <= block_num + 28'b1;
                            if (block_num + 28'b1 === n_blocks) begin
                                state <= DONE;
                                done <= 1'b1;
                            end
                            else begin
                                state <= XOR;
                            end
                        end
                    end
                end

                MEM3: begin
                    state <= MEM2;

                    master_address <= mem_i + (block_num << 28'd4) + block_i;
                    master_read <= 1'b1;

                    master2_address <= (block_num << 28'd4) + block_i;
                    master2_write <= 1'b1;
                    master2_writedata <= block[(block_i>>4'd2)+(block_i<<4'd2)];
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
                        state <= MEM2;
                        r_i <= 4'b0;

                        master_address <= mem_i + (block_num << 28'd4);
                        master_read <= 1'b1;

                        master2_address <= (block_num << 28'd4);
                        master2_write <= 1'b1;
                        master2_writedata <= block[block_i];
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
endmodule: top_aes
