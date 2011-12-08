package org.st.sam.mine;

import java.util.Map;
import java.util.Map.Entry;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;

public class TriggerEffectMiner extends MineLSC {

  public TriggerEffectMiner(short[] trigger, short[] effect) {
    super(Configuration.mineEffect(trigger));
  }

  public static void main(String args[]) throws Exception {
    if (args.length == 1) {
      XESImport xin = new XESImport();
      XLog xlog = xin.readLog(args[0]);
      SLog log = new SLog(xlog);
      
      System.out.println("available event classes:");
      for (Entry<String, Short> ev : log.name2id.entrySet()) {
        System.out.println(ev.getKey()+" -> "+ev.getValue());
      }
      return;
    }
    if (args.length == 4) {
      String logFile = args[0];
      int support = Integer.parseInt(args[1]);
      double confidence = Double.parseDouble(args[2]);

      String triggerString = args[4];
      String events[] = triggerString.split(",");
      short[] trigger = new short[events.length];
      for (int e=0;e<events.length;e++) {
        trigger[e] =  Short.parseShort(events[e]);
      }
      
      System.out.println("mining for scenarios with trigger "+MineLSC.toString(trigger));
      
      TriggerEffectMiner miner = new TriggerEffectMiner(trigger, null);
      miner.mineLSCs(logFile, support, confidence);
      
      System.out.println("found "+miner.getScenarios().values().size()+" scenarios");
      for (SScenario s : miner.getScenarios().values()) {
        System.out.println(s.toString());
      }
    }
  }
}
