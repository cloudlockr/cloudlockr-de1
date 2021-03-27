package com.example.spp;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

//import com.example.bttest.R;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.widget.TextView;
import android.widget.Toast;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;


public class MainActivity extends Activity {
    TextView out;
    private static final int REQUEST_ENABLE_BT = 1;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;

    // Well known SPP UUID
    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    // Insert your server's MAC address
    private static String address = "20:17:01:04:22:27";

    /** Called when the activity is first created. */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        out = (TextView) findViewById(R.id.out);

        out.append("\n...In onCreate()...");

        btAdapter = BluetoothAdapter.getDefaultAdapter();
        CheckBTState();
    }

    @Override
    public void onStart() {
        super.onStart();
        out.append("\n...In onStart()...");
    }

    @Override
    public void onResume() {
        int result = 0;

        super.onResume();

        result = 0;

        out.append("\n...In onResume...\n...Attempting client connect...");

        // Set up a pointer to the remote node using it's address.
        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        // Two things are needed to make a connection:
        //   A MAC address, which we got above.
        //   A Service ID or UUID.  In this case we are using the
        //     UUID for SPP.
        try {
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            AlertBox("Fatal Error", "In onResume() and socket create failed: " + e.getMessage() + ".");
            result = 1;
        }

        // Discovery is resource intensive.  Make sure it isn't going on
        // when you attempt to connect and pass your message.
        btAdapter.cancelDiscovery();

        // Establish the connection.  This will block until it connects.
        if ( result == 0 ) {
            try {
                btSocket.connect();
                out.append("\n...Connection established and data link opened...");
            } catch (IOException e) {
                AlertBox("Fatal Error", "In onResume() and connect() failed" + e.getMessage() + ".");
                result = 1;


            }
        }

        if ( result == 0 ) {

            // Create a data stream so we can talk to server.
            out.append("\n...Sending message to server...");

            try {
                outStream = btSocket.getOutputStream();
            } catch (IOException e) {
                AlertBox("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
            }
        }


        if (  result == 0) {
            String message = "Hello, Jason the \n";
            byte[] msgBuffer = message.getBytes();

            try {
                outStream.write(msgBuffer);
            } catch (IOException e) {
                String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
                if (address.equals("00:00:00:00:00:00"))
                    msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 37 in the java code";
                msg = msg + ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";

                AlertBox("Fatal Error", msg);
            }
        }

        SystemClock.sleep(500);

        if (  result == 0) {
            String message = "great!!!\n";
            byte[] msgBuffer = message.getBytes();

            try {
                outStream.write(msgBuffer);
            } catch (IOException e) {
                String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
                if (address.equals("00:00:00:00:00:00"))
                    msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 37 in the java code";
                msg = msg + ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";

                AlertBox("Fatal Error", msg);
            }
        }

        if ( result == 1 )
        {
            try {
                btSocket.close();
            } catch (IOException e2) {
                AlertBox("Fatal Error", "In onResume() and unable to close socket" + e2.getMessage() + ".");
            }
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        out.append("\n...In onPause()...");

        if (outStream != null) {
            try {
                outStream.flush();
            } catch (IOException e) {
                AlertBox("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
            }
        }

        try     {
            btSocket.close();
        } catch (IOException e2) {
            AlertBox("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        out.append("\n...In onStop()...");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        out.append("\n...In onDestroy()...");
    }

    private void CheckBTState() {
        // Check for Bluetooth support and then check to make sure it is turned on

        // Emulator doesn't support Bluetooth and will return null
        if(btAdapter==null) {
            AlertBox("Fatal Error", "Bluetooth Not supported. Aborting.");
        } else {
            if (btAdapter.isEnabled()) {
                out.append("\n...Bluetooth is enabled...");
            } else {
                //Prompt user to turn on Bluetooth
                Intent enableBtIntent = new Intent(btAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }
    }

    public void AlertBox( String title, String message ){
        new AlertDialog.Builder(this)
                .setTitle( title )
                .setMessage( message + " Press OK to exit." )
                .setPositiveButton("OK", new OnClickListener() {
                    public void onClick(DialogInterface arg0, int arg1) {
                        finish();
                    }
                }).show();
    }
}