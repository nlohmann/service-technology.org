package org.st.sam.log;

import java.util.Arrays;
import com.google.gwt.dev.util.collect.HashSet;

public class SScenario {

  public short pre[];
  public short main[];
  
  public SScenario(short pre[], short main[]) {
    this.pre = pre;
    this.main = main;
  }
  
  public short[] getWord() {
    short[] word = new short[pre.length+main.length];
    for (int e=0; e<pre.length; e++) {
      word[e] = pre[e];
    }
    for (int e=0; e<main.length; e++) {
      word[e+pre.length] = main[e];
    }
    return word;
  }
  
  public String toString() {
    String ret = "";
    for (short e : pre) {
      ret += e+" ";
    }
    ret += " --> ";
    for (short e : main) {
      ret += e+" ";
    }
    return ret;
  }
  
  public static boolean isSubWordOf(short[] word, short[] other, HashSet<Short> visible) {
    
    if (word.length > other.length) return false;
    
    int i=0, j=0;
    while (j<other.length) {
      // skip all letter of other that are not visible
      if (!visible.contains(other[j])) {
        j++;
        continue;
      }
      // 'other' has one visible letter more than 'word', not a subword
      if (i >= word.length) return false;
      // 'other' and 'word' disagree on order of visible letters
      if (word[i] != other[j]) return false;
      i++;
      j++;
    }
    // 'other' had less visible letters than 'word', not a subword
    if (i != word.length) return false;
    
    return true;
  }
  
  public boolean weakerThan_byPreMain(SScenario other) {
    if (pre.length+main.length != other.pre.length+other.main.length) return false;
    if (pre.length <= other.pre.length) return false;
    // this has a longer pre-chart than other, but they have the same length
    // see if the they describe the same scenario
    short[] thisword = new short[pre.length+main.length];
    for (int e = 0; e<pre.length; e++) thisword[e] = pre[e];
    for (int e = 0; e<main.length; e++) thisword[e+pre.length] = main[e];
    
    short[] otherword = new short[other.pre.length+other.main.length];
    for (int e = 0; e<other.pre.length; e++) otherword[e] = other.pre[e];
    for (int e = 0; e<other.main.length; e++) otherword[e+other.pre.length] = other.main[e];

    return Arrays.equals(thisword, otherword);
  }
  
  public boolean weakerThan(SScenario other) {
    return (isSubSequence(other.pre, this.pre) && isSubSequence(this.main, other.main));
  }
  
  public boolean weakerThan2(SScenario other) {
    
    HashSet<Short> visible = new HashSet<Short>();
    for (short e : pre) visible.add(e);
    for (short e : main) visible.add(e);
    
    HashSet<Short> visible_other = new HashSet<Short>();
    for (short e : other.pre) visible_other.add(e);
    for (short e : other.main) visible_other.add(e);
    
    if (isSubWordOf(other.pre, pre, visible_other) && isSubWordOf(main, other.main, visible)) {
      return true;
    }
    boolean result = weakerThan_byPreMain(other);
    
    if (result == false && pre[pre.length-1] == 25 && main[0] == 26 && other.pre[0] == 25 && other.pre.length+other.main.length == 2) {
      System.out.println(this+" weaker than "+other+" "+result);
      System.out.println(isSubWordOf(other.pre, pre, visible_other)+" "+isSubWordOf(main, other.main, visible));
    }
    return result;
  }
  
  public boolean weakerThan_main(SScenario other) {
    if (!Arrays.equals(pre, other.pre)) return false;
    if (this.main.length >= other.main.length) return false;
    for (int i=0; i < this.main.length; i++) {
      // main-charts are different
      if (this.main[i] != other.main[i]) return false;
    }
    // this chart has the same pre-chart as 'other' and the
    // main chart is a prefix of 'other's main-chart
    return true;
  }
  
  public boolean weakerThan_pre(SScenario other) {
    if (!Arrays.equals(main, other.main)) return false;
    if (this.pre.length <= other.pre.length) return false;
    for (int this_i = this.pre.length-1, other_i=other.pre.length-1; 
        other_i >= 0 && this_i >= 0;
        other_i--, this_i--) {
      // main-charts are different
      if (this.pre[this_i] != other.pre[other_i]) return false;
    }
    // this chart has the same main-chart as 'other' and the
    // 'other's pre-chart is a suffix of this pre-chart
    return true;
  }
  
  
  public static boolean isSubSequence(short[] thisChart, short[] otherChart) {
    
    int o = 0;
    for (int t=0; t<thisChart.length; t++) {
      // for each letter 'this[t]' of 'this', find a letter of 'other'
      // starting at the last position
      for ( ; o < otherChart.length; o++) {
        if (thisChart[t] == otherChart[o]) {
          // this[t] has a corresponding letter in other at position 'o'
          break;
        }
      }
      if (o >= otherChart.length) {
        // could not find this[t] in other at any position
        return false;
      }
      o++; // move to next letter of 'other'
    }
    return true;
  }

}
