package org.st.anica.ui;
import java.io.*;
import java.net.*;

public class UDPClient {
    private String hostname = "localhost";
    private int port = 5556;
    private InetAddress ipaddress;
    private DatagramSocket socket;

    public UDPClient() throws IOException {
        socket = new DatagramSocket(); 
        ipaddress = InetAddress.getByName(hostname);
    }
    
    public void send(String s) throws IOException {
        byte[] sendData = s.getBytes();
        DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, ipaddress, port); 
        socket.send(sendPacket);
    }

    protected void finalize() {
        socket.close();
    }
    
    public static void main(String[] args) throws IOException {
        UDPClient e = new UDPClient();
        e.send("Hallo!");
        e.finalize();
    }
}
