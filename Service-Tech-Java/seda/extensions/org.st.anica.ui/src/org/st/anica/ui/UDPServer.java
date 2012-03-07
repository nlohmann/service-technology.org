package org.st.anica.ui;

import java.io.*;
import java.net.*;

public class UDPServer {
    private int port = 5556;
    private DatagramSocket socket;

    public UDPServer() throws IOException {
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
        UDPServer e = new UDPServer();
        String s = e.receive();
        System.out.println("received: "+s);
        e.finalize();
    }
}
