package org.st.sam.mine;

import org.st.sam.log.SLogTree;

public class MineBranchingLSC extends MineLSC {
  
  public MineBranchingLSC() {
    super(Configuration.mineBranching());
  }
  
  public MineBranchingLSC(SLogTree tree) {
    super(Configuration.mineBranching(), tree);
  }

  public static void main(String args[]) throws Exception {
    
    /*
    if (args.length != 3) {
      System.out.println("error, wrong number of arguments");
      System.out.println("usage: java "+MineLSC.class.getCanonicalName()+" <logfile.xes>");
      return;
    }
    
    String srcFile = args[0];
    int support = Integer.parseInt(args[1]);
    double confidence = Double.parseDouble(args[2]);

    */
    String srcFile = "./experiments/crossftp_succinct/crossftp_succinct_traceset.xes.gz";
    int support = 12;
    double confidence = 1.0;
    MineLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(srcFile, support, confidence);
  }
}
