`timescale 1 ps / 1 ps

module tb_aes_encrypt();
    logic clk, rst_n;
    logic slave_waitrequest, slave_read, slave_write;
    logic [3:0] slave_address;
    logic [31:0] slave_readdata, slave_writedata;

    logic [7:0] mem_content [0:527];
    logic [7:0] ref_content [0:255];

    aes_encrypt dut(.*);

    logic [31:0] read_data;

    initial begin
        clk = 1'b0;
        forever #10 clk = ~clk;
    end

    initial begin
        slave_address = 4'b0;
        slave_read = 1'b0;
        slave_write = 1'b0;

        @(posedge clk);
        $readmemh("./mem_content0.memh", mem_content);
        $readmemh("./ref_content0.memh", ref_content);

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'd0;
        slave_writedata = (mem_content[259] << 24) + (mem_content[258] << 16) + (mem_content[257] << 8) + (mem_content[256]);

        @(posedge clk);
        #2;
        slave_address = 4'd1;
        slave_writedata = (mem_content[263] << 24) + (mem_content[262] << 16) + (mem_content[261] << 8) + (mem_content[260]);

        @(posedge clk);
        #2;
        slave_address = 4'd2;
        slave_writedata = (mem_content[267] << 24) + (mem_content[266] << 16) + (mem_content[265] << 8) + (mem_content[264]);

        @(posedge clk);
        #2;
        slave_address = 4'd3;
        slave_writedata = (mem_content[271] << 24) + (mem_content[270] << 16) + (mem_content[269] << 8) + (mem_content[268]);

        @(posedge clk);
        #2;
        slave_address = 4'd4;
        slave_writedata = (mem_content[275] << 24) + (mem_content[274] << 16) + (mem_content[273] << 8) + (mem_content[272]);

        @(posedge clk);
        #2;
        slave_address = 4'd5;
        slave_writedata = (mem_content[279] << 24) + (mem_content[278] << 16) + (mem_content[277] << 8) + (mem_content[276]);

        @(posedge clk);
        #2;
        slave_address = 4'd6;
        slave_writedata = (mem_content[283] << 24) + (mem_content[282] << 16) + (mem_content[281] << 8) + (mem_content[280]);

        @(posedge clk);
        #2;
        slave_address = 4'd7;
        slave_writedata = (mem_content[287] << 24) + (mem_content[286] << 16) + (mem_content[285] << 8) + (mem_content[284]);

        @(posedge clk);
        #2;
        slave_address = 4'd8;
        slave_writedata = 32'h01;

        @(posedge clk);
        @(posedge clk);
        slave_write = 1'b0;

        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        for (int i = 9'd0; i < 9'd256; i++) begin
            assert(dut.sbox[i] === mem_content[i]);
        end

        @(posedge dut.done);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        #2;
        slave_read = 1'b1;
        slave_address = 4'd0;

        @(posedge clk);
        read_data = slave_readdata;
        slave_read = 1'b1;
        slave_address = 4'd1;

        #2;
        assert(slave_readdata[7:0] === ref_content[0]);
        assert(slave_readdata[15:8] === ref_content[1]);
        assert(slave_readdata[23:16] === ref_content[2]);
        assert(slave_readdata[31:24] === ref_content[3]);

        @(posedge clk);
        read_data = slave_readdata;
        slave_read = 1'b1;
        slave_address = 4'd2;

        #2;
        assert(slave_readdata[7:0] === ref_content[4]);
        assert(slave_readdata[15:8] === ref_content[5]);
        assert(slave_readdata[23:16] === ref_content[6]);
        assert(slave_readdata[31:24] === ref_content[7]);

        @(posedge clk);
        read_data = slave_readdata;
        slave_read = 1'b1;
        slave_address = 4'd3;

        #2;
        assert(slave_readdata[7:0] === ref_content[8]);
        assert(slave_readdata[15:8] === ref_content[9]);
        assert(slave_readdata[23:16] === ref_content[10]);
        assert(slave_readdata[31:24] === ref_content[11]);

        @(posedge clk);
        read_data = slave_readdata;
        slave_read = 1'b1;
        slave_address = 4'd3;

        #2;
        assert(slave_readdata[7:0] === ref_content[12]);
        assert(slave_readdata[15:8] === ref_content[13]);
        assert(slave_readdata[23:16] === ref_content[14]);
        assert(slave_readdata[31:24] === ref_content[15]);

        #400;
        $stop;
    end
endmodule: tb_aes_encrypt