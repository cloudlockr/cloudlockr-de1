`timescale 1 ps / 1 ps

module tb_top_aes();
    logic clk, rst_n;
    logic slave_waitrequest, slave_read, slave_write;
    logic [3:0] slave_address;
    logic [31:0] slave_readdata, slave_writedata;
    logic master_waitrequest, master_read, master_write, master_readdatavalid;
    logic [31:0] master_address;
    logic [7:0] master_readdata, master_writedata;

    logic [7:0] mem_content [0:287];

    top_aes dut(.*);

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

    initial begin
        clk = 1'b0;
        forever #10 clk = ~clk;
    end

    initial begin
        slave_address = 4'b0;
        slave_read = 1'b0;
        slave_write = 1'b0;
        rst_n = 1'b1;

        @(posedge clk);
        $readmemh("./mem_content.memh", mem_content);

        @(posedge clk);
        rst_n = 1'b0;
        @(posedge clk);
        @(posedge clk);
        rst_n = 1'b1;

        @(posedge clk);
        #2;
        slave_write = 1'b1;
        slave_address = 4'b0010;
        slave_writedata = 32'b0;

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
        // verify that data in key is correct
        for (int i = 9'd0; i < 9'd16; i++) begin
            assert(dut.key[i] === mem_content[i + 9'd256]);
        end
        // verify that data in key is correct
        for (int i = 9'd0; i < 9'd16; i++) begin
            assert(dut.block[i] === mem_content[i + 9'd272]);
        end

        @(posedge done);

        #400;
        $stop;
    end
endmodule: tb_top_aes