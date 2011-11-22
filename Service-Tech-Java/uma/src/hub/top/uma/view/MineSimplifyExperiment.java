package hub.top.uma.view;

import hub.top.petrinet.PetriNetIO;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;

import java.io.FileNotFoundException;
import java.io.IOException;

public class MineSimplifyExperiment extends MineSimplify {
  
  public MineSimplifyExperiment(String fileName_system_sysPath, String fileName_trace, Configuration config) 
    throws IOException, InvalidModelException, FileNotFoundException
  {
    super(fileName_system_sysPath, fileName_trace, config);
  }
  
  private static String s2n(int step) {
    switch (step) {
      case Result.ORIGINAL: return "org";
      case Result.STEP_UNFOLD: return "unf";
      case Result.STEP_FILTER: return "filt";
      case Result.STEP_EQUIV: return "equ";
      case Result.STEP_FOLD: return "fold";
      case Result.STEP_IMPLIED: return "imp";
      case Result.STEP_CHAINS: return "ch";
      case Result.STEP_FLOWER: return "fl";
      case Result.FINAL: return "sim";
    }
    return Integer.toString(step);
  }
  
  private static String c2s(double value) {
    //(int)(value*100)
    return Double.toString(value);
  }
  
  public static String generateCSVHeader(int type) {
    String result_string = "model;";
    
    if (type == ETYPE_CONFIG) {
      for (int step=Result.ORIGINAL; step <= Result.FINAL; step++) {
        result_string += "P_"+s2n(step)+";T_"+s2n(step)+";F_"+s2n(step)+";c_"+s2n(step)+";t_"+s2n(step)+";";
      }

    } else if (type == ETYPE_ISOLATED) {
      result_string += "P_"+s2n(Result.ORIGINAL)+";T_"+s2n(Result.ORIGINAL)+";F_"+s2n(Result.ORIGINAL)+";c_"+s2n(Result.ORIGINAL)+";";
      result_string += "P_"+s2n(Result.STEP_FOLD)+";T_"+s2n(Result.STEP_FOLD)+";F_"+s2n(Result.STEP_FOLD)+";c_"+s2n(Result.STEP_FOLD)+";";      
      result_string += "P_"+s2n(Result.STEP_IMPLIED)+"1;T_"+s2n(Result.STEP_IMPLIED)+"1;F_"+s2n(Result.STEP_IMPLIED)+"1;c_"+s2n(Result.STEP_IMPLIED)+"1;";
      result_string += "P_"+s2n(Result.STEP_IMPLIED)+"2;T_"+s2n(Result.STEP_IMPLIED)+"2;F_"+s2n(Result.STEP_IMPLIED)+"2;c_"+s2n(Result.STEP_IMPLIED)+"2;";
      result_string += "P_"+s2n(Result.STEP_IMPLIED)+"3;T_"+s2n(Result.STEP_IMPLIED)+"3;F_"+s2n(Result.STEP_IMPLIED)+"3;c_"+s2n(Result.STEP_IMPLIED)+"3;";
      result_string += "P_"+s2n(Result.STEP_IMPLIED)+"4;T_"+s2n(Result.STEP_IMPLIED)+"4;F_"+s2n(Result.STEP_IMPLIED)+"4;c_"+s2n(Result.STEP_IMPLIED)+"4;";
      result_string += "P_"+s2n(Result.STEP_FLOWER)+";T_"+s2n(Result.STEP_FLOWER)+";F_"+s2n(Result.STEP_FLOWER)+";c_"+s2n(Result.STEP_FLOWER)+";";
      
    } else if (type == ETYPE_ACCUMULATED) {

      result_string += "P_"+s2n(Result.ORIGINAL)+";T_"+s2n(Result.ORIGINAL)+";F_"+s2n(Result.ORIGINAL)+";c_"+s2n(Result.ORIGINAL)+";";
      result_string += "prec_"+s2n(Result.ORIGINAL)+";"+"prec_"+s2n(Result.STEP_UNFOLD)+";";
      result_string += "P_"+s2n(Result.STEP_FOLD)+";T_"+s2n(Result.STEP_FOLD)+";F_"+s2n(Result.STEP_FOLD)+";c_"+s2n(Result.STEP_FOLD)+";t_"+s2n(Result.STEP_FOLD)+";";
      result_string += "prec_"+s2n(Result.STEP_FOLD)+";";
      result_string += "P_"+s2n(Result.STEP_IMPLIED)+"1;T_"+s2n(Result.STEP_IMPLIED)+"1;F_"+s2n(Result.STEP_IMPLIED)+"1;c_"+s2n(Result.STEP_IMPLIED)+"1;t_"+s2n(Result.STEP_IMPLIED)+"1;";
      result_string += "prec_"+s2n(Result.STEP_IMPLIED)+"1;";
      result_string += "P_"+s2n(Result.STEP_CHAINS)+";T_"+s2n(Result.STEP_CHAINS)+";F_"+s2n(Result.STEP_CHAINS)+";c_"+s2n(Result.STEP_CHAINS)+";t_"+s2n(Result.STEP_CHAINS)+";";
      result_string += "prec_"+s2n(Result.STEP_CHAINS)+";";
      result_string += "P_"+s2n(Result.STEP_FLOWER)+";T_"+s2n(Result.STEP_FLOWER)+";F_"+s2n(Result.STEP_FLOWER)+";c_"+s2n(Result.STEP_FLOWER)+";t_"+s2n(Result.STEP_FLOWER)+";";
      result_string += "prec_"+s2n(Result.STEP_FLOWER)+";";
      result_string += "traces_rep;";
      result_string += "traces_total;";
      result_string += "t_total";
    }
    
    result_string += "\n";    
    return result_string;
  }
  
  /**
   * Concatenate all intermediate results in a CSV string {@code net size;complexity;time; ...}
   * for all steps recorded in {@link Result}.
   *  
   * @return
   */
  public static String generateCompleteCSVString(String fileName_system_sysPath, Result result) {
    
    /*
    result_string += originalNet.getInfo()+";"+(int)(result.netComplexity_original*100)+";";
    result_string += _size_bp+";"+(_time_buildBP_finish - _time_buildBP_start)+";";
    result_string += _size_fold+";"+(int)(_comp_fold*100)+";"+(_time_equiv_finish - _time_equiv_start)+";";
    result_string += _size_implied+";"+(int)(_comp_implied*100)+";"+(_time_implied_finish - _time_implied_start)+";";
    result_string += _size_chain+";"+(int)(_comp_chain*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    result_string += simplifiedNet.getInfo()+";"+(int)(result.netComplexity_simplified*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    result_string += "\n";
    
    */
    
    String model = fileName_system_sysPath.substring(
        fileName_system_sysPath.lastIndexOf("/")+1,
        fileName_system_sysPath.lastIndexOf("."));
    
    String result_string = model+";";
    
    for (int step=Result.ORIGINAL; step <= Result.FINAL; step++) {
      result_string += result._net_size[step]+";"+c2s(result._complexity[step])+";"+result.getRuntime(step)+";";
    }
    result_string += "\n";
    
    return result_string;
  }
  
  public static void runSingleExperiment_config(String path, String system, String log, Configuration config) throws IOException, InvalidModelException {
    
    System.out.println("====> configuration: "+system+" "+config);

    MineSimplifyExperiment sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, config);
    sim.prepareModel();
    sim.run();
    
    for (int step=Result.ORIGINAL; step <= Result.FINAL; step++) {
      
      if (sim.result._nets[step] != null) {
      
        String targetPath_dot = path+"/"+system+".step"+step+".dot";
        PetriNetIO.writeToFile(sim.result._nets[step], targetPath_dot, PetriNetIO.FORMAT_DOT, 0);
        
        String targetPath_lola = path+"/"+system+".step"+step+".lola";
        PetriNetIO.writeToFile(sim.result._nets[step], targetPath_lola, PetriNetIO.FORMAT_LOLA, 0);
      }
    }
    
    if (sim.getBuild() != null) {
      String targetPath_bp_dot = path+"/"+system+".step"+Result.STEP_UNFOLD+"_bp.dot";
      writeFile(targetPath_bp_dot, sim.getBuild().toDot(sim.debug._weakImpliedConditions), false);
    }
    
    log(path+"/"+getLogFile(ETYPE_CONFIG), generateCompleteCSVString(sim.fileName_system_sysPath, sim.result));
  }
  
  public static void runSingleExperiment_accumulated(String path, String system, String log, int implied_mode) throws IOException, InvalidModelException {
    
    Runtime r = Runtime.getRuntime();
    
    System.out.println("====> accumulated: "+system+" in "+implied_mode);
   
    StringBuilder sb = new StringBuilder();
    
    sb.append(system.substring(0,system.lastIndexOf("."))+"_"+implied_mode+";");

    r.gc();
    MineSimplifyExperiment sim;
    Precision_ETC prec;
    Configuration c;
    
    // Result.STEP_FOLD
    c = new Configuration();
    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    c.filter_threshold = 0.3;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    // original model
    sb.append(sim.result._net_size[Result.ORIGINAL]+";"+c2s(sim.result._complexity[Result.ORIGINAL])+";");
    prec = new Precision_ETC(sim.originalNet, sim.getTraces());
    prec.computePrecision();
    sb.append(prec.getPrecisionTrace()+";");


    // precision of the branching process
    sb.append(sim.viewGen.computePrecision_bp()+";");
    // refolded model
    long refoldTime = sim.result.getRuntime(Result.STEP_UNFOLD)+sim.result.getRuntime(Result.STEP_EQUIV)+sim.result.getRuntime(Result.STEP_FOLD);
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";"+refoldTime+";");
    prec = new Precision_ETC(sim.result._nets[Result.FINAL], sim.getTraces());
    prec.computePrecision();
    sb.append(prec.getPrecisionTrace()+";");
    
    c = new Configuration();
    c.unfold_refold = true;
    c.remove_implied = implied_mode;
    c.abstract_chains = true;
    c.remove_flower_places = true;
    c.filter_threshold = 0.3;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.STEP_IMPLIED]+";"+c2s(sim.result._complexity[Result.STEP_IMPLIED])+";"+sim.result.getRuntime(Result.STEP_IMPLIED)+";");
    prec = new Precision_ETC(sim.result._nets[Result.STEP_IMPLIED], sim.getTraces());
    prec.computePrecision();
    sb.append(prec.getPrecisionTrace()+";");
    
    sb.append(sim.result._net_size[Result.STEP_CHAINS]+";"+c2s(sim.result._complexity[Result.STEP_CHAINS])+";"+sim.result.getRuntime(Result.STEP_CHAINS)+";");
    prec = new Precision_ETC(sim.result._nets[Result.STEP_CHAINS], sim.getTraces());
    prec.computePrecision();
    sb.append(prec.getPrecisionTrace()+";");
    
    sb.append(sim.result._net_size[Result.STEP_FLOWER]+";"+c2s(sim.result._complexity[Result.STEP_FLOWER])+";"+sim.result.getRuntime(Result.STEP_FLOWER)+";");
    prec = new Precision_ETC(sim.result._nets[Result.STEP_FLOWER], sim.getTraces());
    prec.computePrecision();
    sb.append(prec.getPrecisionTrace()+";");
    sb.append(prec.getReplayedCases()+";");
    sb.append(sim.getTraces().size()+";");
    
    sb.append(sim.result.getRuntime(Result.FINAL)+";");
    
    sb.append("\n");
    
    log(path+"/"+getLogFile(ETYPE_ACCUMULATED), sb.toString());
  }
  
  public static void runSingleExperiment_isolated(String path, String system, String log) throws IOException, InvalidModelException {
    
    Runtime r = Runtime.getRuntime();
    
    System.out.println("====> isolated: "+system);
    
    StringBuilder sb = new StringBuilder();
    
    sb.append(system.substring(0,system.lastIndexOf("."))+";");
    
    MineSimplifyExperiment sim;
    Configuration c;
    
    // Result.STEP_FOLD
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();

    sb.append(sim.result._net_size[Result.ORIGINAL]+";"+c2s(sim.result._complexity[Result.ORIGINAL])+";");
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");
    
    // Result.STEP_IMPLIED = REMOVE_IMPLIED_PRESERVE_ALL
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");
    
    // Result.STEP_IMPLIED = REMOVE_IMPLIED_PRESERVE_VISIBLE
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");


    // Result.STEP_IMPLIED = REMOVE_IMPLIED_PRESERVE_CONNECTED
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");

    // Result.STEP_IMPLIED = REMOVE_IMPLIED_PRESERVE_CONNECTED2
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_ILP;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");
    
    // Result.STEP_FLOWER 
    c = new Configuration();
    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = true;
    
    r.gc();
    sim = new MineSimplifyExperiment(path+"/"+system, path+"/"+log, c);
    sim.prepareModel();
    sim.run();
    
    sb.append(sim.result._net_size[Result.FINAL]+";"+c2s(sim.result._complexity[Result.FINAL])+";");

    sb.append("\n");

    log(path+"/"+getLogFile(ETYPE_ISOLATED), sb.toString());
}
  
  public synchronized static void log_new(String logfile, String line) throws IOException {
    writeFile(logfile, line, false);
  }
  
  public synchronized static void log(String logfile, String line) throws IOException {
    
    writeFile(logfile, line, true);
  }
  
  public static final int ETYPE_CONFIG = 0;
  public static final int ETYPE_ISOLATED = 1;
  public static final int ETYPE_ACCUMULATED = 2;
  
  public static void runSingleExperiment(final String path, final String system, final String log, final int type, final Configuration config) throws IOException, InvalidModelException {
    
    switch (type) {
    case ETYPE_CONFIG: 
      runSingleExperiment_config(path, system, log, config);
      break;
    case ETYPE_ISOLATED:
      runSingleExperiment_isolated(path, system, log);
      break;
    case ETYPE_ACCUMULATED:
      for (int mode=Configuration.REMOVE_ILP; mode <= Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED2; mode++) {
        runSingleExperiment_accumulated(path, system, log, mode);
      }
      break;
    }
    
  }
  
  public static String getLogFile(int type) {
    switch (type) {
      case ETYPE_CONFIG: return "results_config.csv";
      case ETYPE_ISOLATED: return "results_isolated.csv";
      case ETYPE_ACCUMULATED: return "results_accumulated.csv";
    }
    return "results.csv";
  }

  
  public static void runExperiment(String path, Configuration config, boolean f, int type) throws IOException, InvalidModelException {
    //DNodeBP.ignoreFoldThreshold = false;
    
    log_new(path+"/"+getLogFile(type), generateCSVHeader(type));
    
            runSingleExperiment(path, "a12f0n00.lola", "a12f0n00.log.txt", type, config); // F
            runSingleExperiment(path, "a12f0n05.lola", "a12f0n05.log.txt", type, config); // F
            runSingleExperiment(path, "a12f0n10.lola", "a12f0n10.log.txt", type, config); // F
            runSingleExperiment(path, "a12f0n20.lola", "a12f0n20.log.txt", type, config); // F
            runSingleExperiment(path, "a12f0n50.lola", "a12f0n50.log.txt", type, config); // F
    
            runSingleExperiment(path, "a22f0n00.lola", "a22f0n00.log.txt", type, config); // F
            runSingleExperiment(path, "a22f0n05.lola", "a22f0n05.log.txt", type, config); // F
    if (!f) runSingleExperiment(path, "a22f0n10.lola", "a22f0n10.log.txt", type, config); // slow
    if (!f) runSingleExperiment(path, "a22f0n20.lola", "a22f0n20.log.txt", type, config); // slow
    if (!f) runSingleExperiment(path, "a22f0n50.lola", "a22f0n50.log.txt", type, config); // slow
    
            runSingleExperiment(path, "a32f0n00.lola", "a32f0n00.log.txt", type, config);
            runSingleExperiment(path, "a32f0n05.lola", "a32f0n05.log.txt", type, config); // F
    if (!f) runSingleExperiment(path, "a32f0n10.lola", "a32f0n10.log.txt", type, config); // slow 
    if (!f) runSingleExperiment(path, "a32f0n20.lola", "a32f0n20.log.txt", type, config); // slow 
    if (!f) runSingleExperiment(path, "a32f0n50.lola", "a32f0n50.log.txt", type, config); // slow
    
    // AMC
            runSingleExperiment(path, "Aandoening_A.lola", "Aandoening_A.log.txt", type, config); // F
            runSingleExperiment(path, "Aandoening_B.lola", "Aandoening_B.log.txt", type, config); // F
            runSingleExperiment(path, "Aandoening_C.lola", "Aandoening_C.log.txt", type, config); // F
            runSingleExperiment(path, "AMC.lola", "AMC.log.txt", type, config); // F
    
    // Heusden
            runSingleExperiment(path, "Afschriften.lola", "Afschriften.log.txt", type, config); // F
            runSingleExperiment(path, "BezwaarWOZ_filtered_All.lola", "BezwaarWOZ_filtered_All.log.txt", type, config); // F

    // Catharina
            runSingleExperiment(path, "Complications.filtered80.lola", "Complications.filtered80.log.txt", type, config); // F
    
    Uma.out.println("Experiment completed.");
  }
  
  public static void main(String[] args) throws IOException, InvalidModelException {

    Configuration c = new Configuration();
    /*
    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp1_foldRefold", c);

    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp2_foldRefold_implied", c);

    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp3_implied", c);

    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = true;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp4_foldRefold_implied_chains", c);
    */
    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    c.filter_threshold = .10;
    //runExperiment("./examples/is11/exp5_complete", c, false, ETYPE_CONFIG);
    //runExperiment("./examples/is11/exp5_complete", null, false, ETYPE_ISOLATED);
    runExperiment("./examples/is11/exp5_complete", null, false, ETYPE_ACCUMULATED);


    //Precision.main(null);
  }
}
