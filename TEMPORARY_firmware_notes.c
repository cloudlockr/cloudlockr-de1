

typedef enum
{
    STATE_1 = 0,
    STATE_2,
    STATE_3,
    // ...
    
} BLUETOOTH_CMD;



void BLUETOOTH_Receive( char ch )
{

    static int state = 0;
    

      case(state)
      0:
      {
        // test for received Data
        case(received char):
          1: move to state for handling msg 1
            return (something that indicates what frontend should send next)
          2: move to state for handling msg 2
          
          // etc...    
      }
      1:{}
      2:{}
      / etc...
    
      (ending state):{  }
    
   
    
    
}
