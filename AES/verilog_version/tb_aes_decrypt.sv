`timescale 1 ps / 1 ps

module tb_aes_decrypt();
    logic clk, rst_n;
    logic slave_waitrequest, slave_read, slave_write;
    logic [3:0] slave_address;
    logic [31:0] slave_readdata, slave_writedata;

    logic [7:0] mem_content [0:783];
    logic [7:0] ref_content [0:255];

    aes_decrypt dut(.*);

    initial begin
        clk = 1'b0;
        forever #10 clk = ~clk;
    end

    initial begin
        slave_address = 4'b0;
        slave_read = 1'b0;
        slave_write = 1'b0;

        // RUN aes.py BEFORE RUNNING THE TESTBENCH
        @(posedge clk);
        $readmemh("./mem_content1.memh", mem_content);
        $readmemh("./ref_content1.memh", ref_content);

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'd4;
        slave_writedata = (mem_content[531] << 24) + (mem_content[530] << 16) + (mem_content[529] << 8) + (mem_content[528]);

        @(posedge clk);
        #2;
        slave_address = 4'd5;
        slave_writedata = (mem_content[535] << 24) + (mem_content[534] << 16) + (mem_content[533] << 8) + (mem_content[532]);

        @(posedge clk);
        #2;
        slave_address = 4'd6;
        slave_writedata = (mem_content[539] << 24) + (mem_content[538] << 16) + (mem_content[537] << 8) + (mem_content[536]);

        @(posedge clk);
        #2;
        slave_address = 4'd7;
        slave_writedata = (mem_content[543] << 24) + (mem_content[542] << 16) + (mem_content[541] << 8) + (mem_content[540]);

        @(posedge clk);
        #2;
        slave_address = 4'd0;
        slave_writedata = (mem_content[515] << 24) + (mem_content[514] << 16) + (mem_content[513] << 8) + (mem_content[512]);

        @(posedge clk);
        #2;
        slave_address = 4'd1;
        slave_writedata = (mem_content[519] << 24) + (mem_content[518] << 16) + (mem_content[517] << 8) + (mem_content[516]);

        @(posedge clk);
        #2;
        slave_address = 4'd2;
        slave_writedata = (mem_content[523] << 24) + (mem_content[522] << 16) + (mem_content[521] << 8) + (mem_content[520]);

        @(posedge clk);
        #2;
        slave_address = 4'd3;
        slave_writedata = (mem_content[527] << 24) + (mem_content[526] << 16) + (mem_content[525] << 8) + (mem_content[524]);

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

        for (int i = 9'd0; i < 9'd256; i++) begin
            assert(dut.r_sbox[i] === mem_content[i + 10'd256]);
        end

        @(posedge dut.done);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        slave_read = 1'b1;
        slave_address = 4'd0;

        #2;
        assert(slave_readdata[7:0] === ref_content[0]);
        assert(slave_readdata[15:8] === ref_content[1]);
        assert(slave_readdata[23:16] === ref_content[2]);
        assert(slave_readdata[31:24] === ref_content[3]);

        @(posedge clk);
        slave_read = 1'b1;
        slave_address = 4'd1;

        #2;
        assert(slave_readdata[7:0] === ref_content[4]);
        assert(slave_readdata[15:8] === ref_content[5]);
        assert(slave_readdata[23:16] === ref_content[6]);
        assert(slave_readdata[31:24] === ref_content[7]);

        @(posedge clk);
        slave_read = 1'b1;
        slave_address = 4'd2;

        #2;
        assert(slave_readdata[7:0] === ref_content[8]);
        assert(slave_readdata[15:8] === ref_content[9]);
        assert(slave_readdata[23:16] === ref_content[10]);
        assert(slave_readdata[31:24] === ref_content[11]);

        @(posedge clk);
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
endmodule: tb_aes_decrypt