module  GraphicsController_Verilog (
 
		input unsigned [15:0] AddressIn,								// CPU 32 bit address bus
		input unsigned [15:0] DataInFromCPU, 						// 16 bit data bus from CPU

  		input Clk, Reset_L,
		input AS_L, UDS_L, LDS_L, RW,

// VGA display driver signals (the circuit actually displaying the contents of the frame buffer)

		input GraphicsCS_L,	 											// CS from main computer address decoder
		input VSync_L,														// Vertical sync from VGA controller
		input unsigned [15:0] SRam_DataIn,							// 16 bit data bus in from Sran
		
// Sram output signals
		
		output reg unsigned [9:0] VScrollValue,  					// scroll value for terminal emulation (allows screen scrolling up/down)
		output reg unsigned [9:0] HScrollValue,   				// scroll value for terminal emulation (allows screen scrolling left/right)

// Data bus back to CPU	
		output reg unsigned [15:0] DataOutToCPU,					// 16 bit data bus to CPU

// Memory output signals
// Memory holds the graphics image that we see and draw - we have to dri these signals to create the image as part of a state machine
// Memory is 16 wide, split into 2 x 8 bit halves
// each location is thus 2 bytes and holds two pixels from our image
	
		output reg unsigned [17:0] Sram_AddressOut,				// graphics controller address out 256k locations = 18 address lines
		output reg unsigned [15:0] Sram_DataOut,					// graphics controller Data out
		
		output reg Sram_UDS_Out_L, 									// Upper Data Strobe : Drive to '0' if graphics wants to read/write from/to lower byte in memory
		output reg Sram_LDS_Out_L, 									// Lower Data Strobe : Drive to '0' if graphics wants to read/write from/to upper byte in memory
		output reg Sram_RW_Out,											// Read/Write : Drive to '0' if graphics wants to write or '1' if graphics wants to read											
		
		output reg unsigned [5:0] ColourPalletteAddr,			// an address connected to programmable colour pallette (64 colours)
		output reg unsigned [23:0] ColourPalletteData,			// 24 bit 00RRGGBB value to write to the colour pallette
		output reg ColourPallette_WE_H								// signal to actually write to colour pallette (data and address must be valid at this time)
	);
	
		// WIRES/REGs etc


	reg signed [15:0] X1, Y1, X2, Y2, Colour, BackGroundColour, Command;			// registers
	reg signed [15:0] Colour_Latch;									// holds data read from a pixel

	// signals to control/select the registers above
	reg  	X1_Select_H,
			X2_Select_H, 
			Y1_Select_H,
			Y2_Select_H,
			Command_Select_H, 
			Colour_Select_H,
			BackGroundColour_Select_H;
	
	reg CommandWritten_H, ClearCommandWritten_H;						// signals to control that a command request has been logged
	reg Idle_H, SetBusy_H, ClearBusy_H;									// signals to control status of the graphics chip				
	
	// Temporary Asynchronous signals that drive the Ram (made synchronous in a register for the state machine)
	// your Verilog code should drive these signals not the real Sram signals.
	// These signals are copied to the real Sram signals with each clock
	
	reg unsigned [17:0] Sig_AddressOut;
	reg unsigned [15:0] Sig_DataOut; 
	
	reg Sig_UDS_Out_L, 
		 Sig_LDS_Out_L, 
		 Sig_RW_Out, 
		 Sig_UDS_Out_Temp_L, 
		 Sig_LDS_Out_Temp_L, 
		 Sig_RW_Out_Temp, 
		 Sig_Busy_H;
		 
	// signal to store/latch the colour read from a pixel
	reg Colour_Latch_Load_H;
	reg unsigned [15:0] Colour_Latch_Data;		// register to hold the two pixels read from a memory location in frame buffer
	
		// Colour Pallette signals
	reg unsigned [7:0] Sig_ColourPalletteAddr;	// address
	reg unsigned [31:0] Sig_ColourPalletteData;	// data
	reg Sig_ColourPallette_WE_H;						// write enable to store new colour pallette data

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// States and Parameters for State machine
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// registers that hold the current state and next state of your state machine at the heart of our graphcis controller

	reg unsigned [7:0] CurrentState, 
							 NextState;
		
	// here are some state numbers associated with some functionality already present in the graphics controller, e.g.
	// writing a pixel, reading a pixel, programming a colour pallette
	//
	// you will be adding new states so make sure you have unique values for each state (no duplicate values)
	// e.g. DrawHLine does not do anything yet - you have add the code to that state to draw a line
	
	parameter Idle	= 8'h00;										// main waiting State
	parameter ProcessCommand = 8'h01;						// State is figure out command
	parameter DrawHLine = 8'h02;			 	 				// State for drawing a Horizontal line
	parameter DrawVline = 8'h03;			 	 				// State for drawing a Vertical line
	parameter DrawLine = 8'h04;				 	 			// State for drawing any line
	parameter DrawPixel = 8'h05;							 	// State for drawing a pixel
	parameter ReadPixel = 8'h06;							 	// State for reading a pixel
	parameter ReadPixel1 = 8'h07;							 	// State for reading a pixel
	parameter ReadPixel2 = 8'h08;							 	// State for reading a pixel
	parameter PalletteReProgram = 8'h09;					// State for programming a pallette colour
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Commands values that can be written to command register by CPU to get graphics controller to draw a shape
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	parameter Hline = 16'h0001;							 	// command is draw Horizontal line
	parameter Vline = 16'h0002;								// command is draw Vertical line
	parameter ALine = 16'h0003;								// command is draw any line
	parameter PutPixel = 16'h000a;							// command to draw a pixel
	parameter GetPixel = 16'h000b;							// command to read a pixel
	parameter ProgramPallette = 16'h0010;					// command is program one of the 256 pallettes with a new RGB value
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Secondary address decoder within chip
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(AddressIn, GraphicsCS_L, RW, AS_L, Idle_H)	begin
		// these are important "default" values for the signals. Remember back in VHDL lectures how we supply default values for signals
		// to stop the VHDL compiler inferring latches
		// these defaults are the "inactive" values for those signals. They get overridden below.

		X1_Select_H 					= 0;
		Y1_Select_H 					= 0;
		X2_Select_H 					= 0;
		Y2_Select_H 					= 0;
		Colour_Select_H 				= 0;
		BackGroundColour_Select_H 	= 0;
		Command_Select_H 				= 0;

		// Base address of the ARM lightweight bridge is hex FF200000. All registers are this addresss + Offset
		// if 16 bit Bridge outputs addres in range hex 0000 - 00FF then Graphics chip will be be accessed
	
		if((GraphicsCS_L == 0) && (RW == 0) && (AS_L == 0) && (AddressIn[15:8] == 8'h00)) begin		
			if(AddressIn[7:1] == 7'b0000_000)			Command_Select_H = 1;							// Command reg is at address offset 0
			else if (AddressIn[7:1] == 7'b0000_001) 	X1_Select_H = 1;									// X1 reg is at address offset 2
			else if (AddressIn[7:1] == 7'b0000_010)	Y1_Select_H = 1;									// Y1 reg is at address offset 4
			else if (AddressIn[7:1] == 7'b0000_011)	X2_Select_H = 1;									// X2 reg is at address offset 6
			else if (AddressIn[7:1] == 7'b0000_100)	Y2_Select_H = 1;									// Y2 reg is at address offset 8
			else if (AddressIn[7:1] == 7'b0000_111)	Colour_Select_H = 1;								// Colour reg is at address offset hex 0E
			else if (AddressIn[7:1] == 7'b0001_000) 	BackGroundColour_Select_H = 1;				// Background colour reg at address offset hex 10
		end
	end
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This process sets CommandWritten_H to '1' when CPU writes to Graphics Command register
// in other words it kick starts the graphics controller into doing something when CPU gives a command to the graphics controller
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	always@(posedge Clk) begin
		if(Reset_L == 0) 										// clear all registers and relevant signals on reset (asynchronous to clock)
			CommandWritten_H <= 1;							// SIMULATION ONLY set back to 0 after
		else begin
			if(Command_Select_H == 1) 						// when CPU writes to command register
				CommandWritten_H <= 1;
			else if(ClearCommandWritten_H == 1) 		// signal to clear the register
				CommandWritten_H <= 0;
		end
	end	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Read Status  -activated when CPU reads status reg of graphics chip
// when bit 0 = 1, device is Idle and ready to receive command
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(*) begin
		DataOutToCPU = 16'bZZZZZZZZZZZZZZZZ;								// default is tri State data-out bus to CPU
		
		if(GraphicsCS_L == 0 && RW == 1 && AS_L == 0) begin 
			if(AddressIn[15:1] == 15'b0) 										// read of status register at offset 0
				DataOutToCPU = {15'b0, Idle_H};							   // leading 15 bits of 0 plus Idle status on bit 0
			else if(AddressIn[15:1] == 15'b0000_0000_0000_111) 		// read of colour register hex 0e/0f
				DataOutToCPU = Colour_Latch ;
		end
	end

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Busy_Idle process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0) 								// clear all registers and relevant signals on reset (asynchronous to clock)
			Idle_H <= 1;								// make sure CPU can read graphics is Idle
		else begin										// when graphics starts to always@ a command, mark it's status as busy (not Idle)
			if(SetBusy_H == 1) 						// when CPU writes to command register		
				Idle_H <= 0;
			else if(ClearBusy_H == 1) 
				Idle_H <= 1;
		end
	end
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// X1 process and register
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0) 
			X1 <= 16'h0 ;								// for SIMULATION ONLY
		else begin
			if(X1_Select_H == 1) begin	
				if(UDS_L == 0) 
					X1[15:8] <= DataInFromCPU[15:8];		
				if(LDS_L == 0) 
					X1[7:0] <= DataInFromCPU[7:0];
			end
		end
	end
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Y1 process and register update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin	
		if(Reset_L == 0) 
			Y1 <= 16'h0 ;			// for SIMULATION ONLY
		else begin
			if(Y1_Select_H == 1) begin
				if(UDS_L == 0) 
					Y1[15:8] <= DataInFromCPU[15:8];	
				if(LDS_L == 0) 
					Y1[7:0] <= DataInFromCPU[7:0];
			end
		end
	end
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// X2 process and register update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0)
			X2 <= 16'h0400 ;						// reset to 1024 pixels
		else begin
			if(X2_Select_H == 1) begin				
				if(UDS_L == 0) 
					X2[15:8] <= DataInFromCPU[15:8];	
				if(LDS_L == 0) 
					X2[7:0] <= DataInFromCPU[7:0];
			end	
		end
	end		
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Y2 process and register update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0) 
			Y2 <= 16'h0200 ;							// 512 pixels
		else begin
			if(Y2_Select_H == 1) begin				
				if(UDS_L == 0) 
					Y2[15:8] <= DataInFromCPU[15:8];	
				if(LDS_L == 0) 
					Y2[7:0] <= DataInFromCPU[7:0];
			end
		end
	end	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colour Reg process and update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0) 
			Colour <= 16'h4 ;					// Colour Pallette number 4 (blue) so screen is erased to blue on reset
		else begin
			if(Colour_Select_H == 1) begin				
				if(UDS_L == 0) 
					Colour[15:8] <= DataInFromCPU[15:8];
				if(LDS_L == 0) 
					Colour[7:0] <= DataInFromCPU[7:0];
			end
		end
	end	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Background Colour Reg process and update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(BackGroundColour_Select_H == 1) begin				
			if(UDS_L == 0) 
				BackGroundColour[15:8] <= DataInFromCPU[15:8];
			if(LDS_L == 0) 
				BackGroundColour[7:0] <= DataInFromCPU[7:0];
		end
	end		
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command process and register update
//
// IMPORTANT - CPU writes to this register to ask the graphics controller to "draw something"
// you must ensure you have set up any relevent registers such as x1, y1 and colour (for drawing a pixel for example)
// or x1,y1,x2,y2 registers for drawing a line etc
//
// As soon as CPU writes to the command register the graphics chip will start to draw the shape
// make sure you check the status of the graphics chip to make sure it is idle before writing to ANY of it's registers
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0) 							// clear all registers and relevant signals on reset (asynchronous to clock)
			Command <= 16'h0;
		else begin
			if(Command_Select_H == 1) begin				
				if(UDS_L == 0) 
					Command[15:8] <= DataInFromCPU[15:8];
				if(LDS_L == 0) 
					Command[7:0] <= DataInFromCPU[7:0];
			end
		end 
	end	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colour Latch process and register update (used for reading pixel)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Colour_Latch_Load_H == 1)
			Colour_Latch <= Colour_Latch_Data;
	end		
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	State Machine Registers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	always@(posedge Clk) begin
		if(Reset_L == 0)
			CurrentState <= Idle; 
		else begin
			CurrentState <= NextState;

// Make outputs to the Frame Buffer Memory (Sram) synchronous
			
			Sram_AddressOut 		<= Sig_AddressOut; 
			Sram_DataOut 			<= Sig_DataOut;
			Sram_UDS_Out_L 		<= Sig_UDS_Out_L;
			Sram_LDS_Out_L 		<= Sig_LDS_Out_L;
			Sram_RW_Out				<= Sig_RW_Out;
			
			// Graphics scroll signal updates
			VScrollValue 			<= 10'b0; 			// no scrolling implemented so output 0
			HScrollValue			<= 10'b0; 			// no scrolling implemented so output 0
			
					
			ColourPalletteAddr	<= Sig_ColourPalletteAddr[5:0];
			ColourPalletteData	<= Sig_ColourPalletteData[23:0];
			ColourPallette_WE_H	<= Sig_ColourPallette_WE_H; 
		end 
	end
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// next State and output logic
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	always@(*) begin
	 
	// start with default values for everything and override as necessary, so we do not infer storage for signals inside this always@
	
		Sig_AddressOut 					= 0;
		Sig_DataOut 						= {Colour[7:0], Colour[7:0]};
		Sig_UDS_Out_L 						= 1;
		Sig_LDS_Out_L 						= 1;
		Sig_RW_Out 							= 1;
		
		ClearBusy_H 						= 0;
		SetBusy_H							= 0;
		ClearCommandWritten_H			= 0;
		Sig_Busy_H							= 1;				// default is device is busy
		
		Colour_Latch_Load_H				= 0;
		Colour_Latch_Data					= 0;
			
		Sig_ColourPalletteAddr			= 0;
		Sig_ColourPalletteData			= 0;
		Sig_ColourPallette_WE_H			= 0; 

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IMPORTANT we have to define what the default NEXT state will be. In this case we the state machine
		// will return to the IDLE state unless we override this with a different one
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		NextState = Idle;
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				
		if(CurrentState == Idle ) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			ClearBusy_H = 1;							// mark status as Idle
			Sig_Busy_H = 0;							// show graphics outside world that it is NOT busy
			
			if(CommandWritten_H == 1) begin
				if(AS_L == 0) 							// wait for CPU to finish writing the command
					NextState = Idle;
				else
					NextState = ProcessCommand;
			end
		end
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
		else if(CurrentState == ProcessCommand) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			SetBusy_H = 1;								// set the busy status of the graphics chip
			ClearCommandWritten_H = 1;				// clear the command now that we are always@ing it

			// decide what command CPU wrote and move to a new state to deal with that command
			
			if(Command == PutPixel) 
				NextState = DrawPixel;
			else if(Command == GetPixel) 
				NextState = ReadPixel;
			else if(Command == ProgramPallette) 
				NextState = PalletteReProgram;	
			else if(Command == Hline) 
				NextState = DrawHLine;	
			else if(Command == Vline) 
				NextState = DrawVline;
			else if(Command == ALine) 
				NextState = ALine;	
				
			// add other code to process any new commands here e.g. draw a circle if you decide to implement that
			// or draw a rectangle etc
					
		end
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == PalletteReProgram) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This state is responsible for programming 1 of the 64 colour pallettes with a new RRGGBB value
// Your program should have written the pallette number to be programmed into the "Colour" register 
// and the 24 bit RRGGBB value to be programmed into the pallette into X1 (most significant 8 bits i.e. RR) 
// and Y1 (least significant 16 bits i.e. GGBB) before writing to the command register with a "program pallette" command)

			Sig_ColourPalletteAddr 	= Colour[7:0];		// pallette number must be in Colour Register
			Sig_ColourPalletteData	= {X1, Y1};			// 00RRGGBB		

			// to avoid flicker on screen, we only reprogram a pallette during the vertical sync period, 
			// i.e. at the end of the last frame and before the next one
			
			if(VSync_L == 0) begin								// if VGA is not displaying at this time, begin we can program pallette, otherwise wait for video blanking during Hsync
				Sig_ColourPallette_WE_H	 = 1;
				NextState = Idle;
			end
			
			else
				NextState = PalletteReProgram;				// stay here until pallette reprogrammed
		end	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == DrawPixel) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This state is responsible for drawing a pixel to an X,Y coordinate on the screen
// Your program/CPU should have written the colour pallette number to the "Colour" register 
// and the coords to the x1 and y1 register before writing to the command register with a "Draw pixel" command)
		
// the address of the pixel is formed from the 9 bit y coord that indicates a "row" (1 out of a maximum of 512 rows)
// coupled with a 9 bit x or column address within that row. Note a 9 bit X address is used for a maximum of 1024 columns or horizontal pixels
// You might thing that 10 bits would be required for 1024 columns and you would be correct, except that the address we are issuing
// holds two pixels (the memory us 16 bit wide remember so each location/address is that of 2 pixels)
				

			Sig_AddressOut 	= {Y1[8:0], X1[9:1]};		// 8 bit X address even though it goes up to 1024 which would mean 10 bits, because each address = 2 pixles/bytes
			Sig_RW_Out			= 0;
				
			if(X1[0] == 1'b0)										// if the address/pixel is an even numbered one
				Sig_UDS_Out_L 	= 0;								// enable write to upper half of Sram data bus
			else
				Sig_LDS_Out_L 	= 0;								// else write to lower half of Sram data bus

			NextState = Idle;
		end
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == ReadPixel) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// present the X/Y address to the memory chip as we did when writing a pixel (see above)
			// it will take 2 clock to get the data back
			// 1 clock will be required for the state machine here to output these signal to the sram (on next risgin edge when we change to state ReadPixel1)
			// plus 1 more clock for the synchronous ram to latch the address and respond with the data (when we change to the state ReadPixel2)

			Sig_AddressOut 	= {Y1[8:0], X1[9:1]};							// 8 bit X address even though it goes up to 1024 which would mean 10 bits, because each address = 2 pixles/bytes
			
			Sig_UDS_Out_L 	= 0;														// enable read from  upper half of Sram data bus
			Sig_LDS_Out_L 	= 0;					

			NextState = ReadPixel1;
		end
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == ReadPixel1)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// when we reach this state, graphics controller will have output address etc to the sram (but the sram will not have clocked it in yet)
			// (it's synchronous ram remember) so on the next clock (next state), it will respond to the address here
			
			NextState = ReadPixel2;	// dummy state to allow signals to reach video ram frame buffer

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == ReadPixel2) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// when we reach this state, sram frame buffer will have clocked in the signals above and after a few nS will be outputting the data
			// for us to grab

			Colour_Latch_Load_H 	= 1;									// set up signal to store sram data
			Colour_Latch_Data[7:0] = SRam_DataIn[7:0];			// grab lower byte

			if(X1[0] == 1'b0)												// if the address/pixel is an even numbered one
				Colour_Latch_Data[7:0] = SRam_DataIn[15:8];		// grab upper byte instead			
			
			NextState = Idle ;
		end
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == DrawHLine) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// TODO in your project
			NextState = Idle;
		end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == DrawVline) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			// TODO in your project
			NextState = Idle;
		end
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if(CurrentState == DrawLine) begin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			// TODO in your project
			NextState = Idle;			
		end
	end
endmodule

	
	