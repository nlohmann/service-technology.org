package org.st.anica.ui;

import java.io.*;
import java.net.*;

import org.json.JSONException;
import org.json.JSONObject;

public class UDPServer {
    
    private DatagramSocket socket;

    public UDPServer(int port) throws IOException {
        socket = new DatagramSocket(port);
    }
    
    public String receive() throws IOException {
        byte[] receiveData = new byte[1024];

        DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length); 

        // this call is blocking
        socket.receive(receivePacket); 

        return new String(receivePacket.getData());
    }

    protected void finalize() {
        socket.close();
    }
    
    public static void main(String[] args) throws IOException {
        UDPServer e = new UDPServer(5556);
        
        while (true) {
          String s = e.receive();
          
          System.out.println("received: "+s);
          
          try {
            JSONObject j_s = new JSONObject(s);
            
            if (j_s.has("command") && j_s.get("command").equals("assignment")) {
            
              JSONObject reply = new JSONObject();
              
              String names[] = JSONObject.getNames(j_s);
              for (String a : names) {
                if (a.equals("command")) continue;
                
                reply.put(a, "HIGH");
              }
              
              reply.put("command", "assignment");
              
              UDPClient c = new UDPClient("localhost", 5555);
              c.send(reply.toString());
              c.finalize();
            }
            
          } catch (JSONException ex) {
            System.err.println(ex);
          }

        }
        //e.finalize();
    }
}
