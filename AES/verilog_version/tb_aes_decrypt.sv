`timescale 1 ps / 1 ps

module tb_aes_decrypt();
    logic clk, rst_n;
    logic slave_waitrequest, slave_read, slave_write;
    logic [3:0] slave_address;
    logic [31:0] slave_readdata, slave_writedata;
    logic master_waitrequest, master_read, master_write, master_readdatavalid;
    logic [31:0] master_address;
    logic [7:0] master_readdata, master_writedata;
    logic master2_waitrequest, master2_read, master2_write, master2_readdatavalid;
    logic [31:0] master2_address;
    logic [7:0] master2_readdata, master2_writedata;

    logic [7:0] mem_content [0:783];
    logic [7:0] text_content [0:255];
    logic [7:0] ref_content [0:255];

    logic [4:0] num_blocks;

    aes_decrypt dut(.*);

    always_ff @(posedge clk) begin
        if (master_read) begin
            master_readdatavalid <= 1'b1;
            master_waitrequest <= 1'b0;
            master_readdata <= mem_content[master_address];
        end
        else begin
            master_readdatavalid <= 1'b0;
            master_waitrequest <= 1'b1;
        end
    end

    always_ff @(posedge clk) begin
        if (master2_write) begin
            master2_waitrequest <= 1'b0;
            text_content[master2_address] <= master2_writedata;
        end
        else begin
            master2_waitrequest <= 1'b1;
        end
    end

    initial begin
        clk = 1'b0;
        forever #10 clk = ~clk;
    end

    initial begin
        slave_address = 4'b0;
        slave_read = 1'b0;
        slave_write = 1'b0;
        rst_n = 1'b1;

        num_blocks = 5'd15;

        @(posedge clk);
        $readmemh("./mem_content1.memh", mem_content);
        $readmemh("./ref_content1.memh", ref_content);

        @(posedge clk);
        rst_n = 1'b0;
        @(posedge clk);
        @(posedge clk);
        rst_n = 1'b1;

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'b0001;
        slave_writedata = 32'b0;

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'b0010;
        slave_writedata = 32'b0;

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'b0011;
        slave_writedata = num_blocks << 4;

        @(posedge clk);
        #2;
        assert(dut.word2 === 32'b0);
        slave_address = 4'b0000;
        slave_writedata = 32'h01;

        @(posedge clk);
        @(posedge clk);
        slave_write = 1'b0;

        @(posedge dut.mem_i === 9'd287);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        // verify that data in sbox is correct
        for (int i = 9'd0; i < 9'd256; i++) begin
            assert(dut.sbox[i] === mem_content[i]);
        end

        @(posedge dut.done);
        // verify that ciphertext is correct
        for (int i = 9'd0; i < (num_blocks << 4); i++) begin
            assert(text_content[i] === ref_content[i]);
        end

        #400;
        $stop;
    end
endmodule: tb_aes_decrypt