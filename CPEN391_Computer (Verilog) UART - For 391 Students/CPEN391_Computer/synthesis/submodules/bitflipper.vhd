library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity bit_flipper is
port (
    clk: in std_logic;
    reset_n: in std_logic;
    addr: in std_logic_vector(1 downto 0);
    rd_en: in std_logic;
    wr_en: in std_logic;
    readdata: out std_logic_vector(31 downto 0);
    writedata: in std_logic_vector(31 downto 0)
);
end bit_flipper;

architecture rtl of bit_flipper is
    signal saved_value: std_logic_vector(31 downto 0);
begin
    
    --saved_value
    process (clk)
    begin
        if rising_edge(clk) then
            if (reset_n = '0') then
                saved_value <= (others => '0');
            elsif (wr_en = '1' and addr = "00") then
                saved_value <= writedata;
            elsif (wr_en = '1' and addr = "01") then
                saved_value <= std_logic_vector(unsigned(saved_value) + 1);
            end if;
        end if;
    end process;
    
    --readdata
    process (rd_en, addr, saved_value)
    begin
        readdata <= (others => '-');
        if (rd_en = '1') then
            if (addr = "00") then
                -- bit-flip
                for i in 0 to 31 loop
                    readdata(i) <= saved_value(31-i);
                end loop;
            elsif (addr = "01") then
                readdata <= saved_value;
            elsif (addr = "10") then
                readdata <= not saved_value;
            end if;
        end if;
    end process;
end rtl;