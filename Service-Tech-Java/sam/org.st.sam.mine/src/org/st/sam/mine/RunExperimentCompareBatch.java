package org.st.sam.mine;

public class RunExperimentCompareBatch {
  
  public static void main(String args[]) throws Exception {
    
    RunExperimentCompare exp;
    
//    exp = new RunExperimentCompare();
//    exp.setParameters("./experiments/p_Afschriften/", "Afschriften_agg.xes.gz", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
//    exp.experiment();
//    
//    exp = new RunExperimentCompare();
//    exp.setParameters("./experiments/p_BezwaarWOZ_filtered/", "BezwaarWOZ_filtered.xml.zip", 1.0 /*fract*/, 10 /*supp*/, 1.0 /* conf */);
//    exp.experiment();
    
//    exp = new RunExperimentCompare();
//    exp.setParameters("./experiments/columba_ext/", "columba_ext_resampled2_agg.xes.gz", 1.0 /*fract*/, 20 /*supp*/, 1.0 /* conf */);
//    exp.experiment();
//    
    exp = new RunExperimentCompare();
    exp.setParameters("./experiments/crossftp_succinct/", "crossftp_succinct_traceset.xes.gz", 1.0 /*fract*/, 12 /*supp*/, 1.0 /* conf B */, 1.0 /* conf L */);
    exp.experiment();

//    exp = new RunExperimentCompare();
//    exp.setParameters("./experiments/jeti_invariants/", "jeti_invariants.xes.gz", 1.0 /*fract*/, 15 /*supp*/, 1.0 /* conf */);
//    exp.experiment();
//    
//    exp = new RunExperimentCompare();
//    exp.setParameters("./experiments/jeti/", "jeti.xes.gz", 1.0 /*fract*/, 5 /*supp*/, 1.0 /* conf */);
//    exp.experiment();
  }

}
