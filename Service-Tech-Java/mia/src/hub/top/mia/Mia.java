package hub.top.mia;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.BitSet;

import javax.management.RuntimeErrorException;

public class Mia {

	public static void main(String args[]) {
		// usage: mia n_old.owfn n_new.owfn
		String str_n_old, str_n_new, s;
		
		try {
		if (args.length != 2) {
			System.out.println("Usage: mia n_old.owfn n_new.owfn");
		} else {
			str_n_old = args[0];
			str_n_new = args[1];
			
			PetriNet n_old = PetriNetIO.readNetFromFile(str_n_old);
            PetriNet n_new = PetriNetIO.readNetFromFile(str_n_new);
           
            // exec mia to compute global jumper transitions
			Process process = Runtime.getRuntime().exec("mia " + str_n_old + " " + str_n_new);
			
			BufferedReader stdInput = new BufferedReader(new 
	                 InputStreamReader(process.getInputStream()));

	        BufferedReader stdError = new BufferedReader(new 
	                 InputStreamReader(process.getErrorStream()));
	            
	        //System.out.println("Here is the standard output of the command:\n");
            while ((s = stdInput.readLine()) != null) {
            	// ignore first caracter
            	String m_old, m_new;
            	int begin, end;
            	
            	//System.out.println(s);
            	
            	begin = end = 1;
            	while (s.charAt(end) != ']') {
            		end++;
            	}
            	m_old = s.substring(begin, end);
            	
            	
            	while (s.charAt(end) != '[') {
            		end++;
            	}
            	
            	begin = end + 1;
            	while (s.charAt(end) != ']') {
            		end++;
            	}
            	
            	m_new = s.substring(begin, end);
            	
                System.out.println(s + " " + m_old + "*" + m_new);
                
            	System.out.println(s);
            }
            
            // read any errors from the attempted command - not of interest at the moment
            while ((s = stdError.readLine()) != null) {
            }
			

            // read the output from the command - gives global jumper transitions
            System.out.println("Here is the standard output of the command:\n");
            while ((s = stdInput.readLine()) != null) {
                System.out.println(s);
                // parse global jumper transitions - convert them to bitset representation
            }
            
            System.out.println(n_old + " " + n_new);
		}
		
		/*
		// parse command line
		ArrayList<SafeMarking> global_states = new ArrayList<SafeMarking>();
		
		BitSet m1_old = new BitSet();
		m1_old.set(0);
		m1_old.set(1);
		m1_old.set(2);
		
		BitSet m1_new = new BitSet();
		m1_new.set(0);
		m1_new.set(1);
		m1_new.set(2);
		
		BitSet m2_old = new BitSet();
		m2_old.set(0);
		m2_old.set(1);
		m2_old.set(3);
		
		BitSet m2_new = new BitSet();
		m2_new.set(0);
		m2_new.set(1);
		m2_new.set(3);
		
		BitSet m3_old = new BitSet();
		m3_old.set(1);
		m3_old.set(2);
		m3_old.set(4);
		
		BitSet m3_new = new BitSet();
		m3_new.set(1);
		m3_new.set(2);
		m3_new.set(4);
		
		BitSet m4_old = new BitSet();
		m4_old.set(4);
		m4_old.set(6);
		m4_old.set(7);
		
		BitSet m4_new = new BitSet();
		m4_new.set(4);
		m4_new.set(6);
		m4_new.set(7);
		
		
		global_states.add(new SafeMarking(m1_old, m1_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m2_old, m2_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m3_old, m3_new, SafeMarking.LEAF));
		global_states.add(new SafeMarking(m4_old, m4_new, SafeMarking.LEAF));
		
		StateTree tree = new StateTree();
		tree.buildTree(global_states);
		tree.traverseTree();
		*/
		
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
