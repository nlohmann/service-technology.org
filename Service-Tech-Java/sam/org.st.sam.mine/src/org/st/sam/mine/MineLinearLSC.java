package org.st.sam.mine;

import org.st.sam.log.SLogTree;

public class MineLinearLSC extends MineLSC {
  
  public MineLinearLSC() {
    super(MineLSC.MODE_LINEAR);
  }
  
  public MineLinearLSC(SLogTree tree) {
    super(MineLSC.MODE_LINEAR, tree);
  }

  public static void main(String args[]) throws Exception {
    if (args.length != 3) {
      System.out.println("error, wrong number of arguments");
      System.out.println("usage: java "+MineLSC.class.getCanonicalName()+" <logfile.xes>");
      return;
    }
    
    String srcFile = args[0];
    int support = Integer.parseInt(args[1]);
    double confidence = Double.parseDouble(args[2]);
    MineLSC miner = new MineLinearLSC();
    miner.mineLSCs_writeResults(srcFile, support, confidence);
  }
}
