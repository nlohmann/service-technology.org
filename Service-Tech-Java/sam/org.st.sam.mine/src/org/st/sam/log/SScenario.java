package org.st.sam.log;


public class SScenario {
  
  private static int _scenarioCounter = 0;

  public short pre[];
  public short main[];
  
  public int id = _scenarioCounter++;
  
  public int support = -1;
  
  public SScenario(short pre[], short main[], int alphabetSize) {
    this(pre, main, alphabetSize, -1);
  }
  
  public SScenario(short pre[], short main[], int alphabetSize, int support) {
    this.pre = pre;
    this.main = main;
    this.support = support;
    

    setWord();
    setAlphabets(alphabetSize);
  }
  
  public short[] word = null;
  
  public short[] setWord() {
    
    if (word == null) {
      word = new short[pre.length+main.length];
      for (int e=0; e<pre.length; e++) {
        word[e] = pre[e];
      }
      for (int e=0; e<main.length; e++) {
        word[e+pre.length] = main[e];
      }
    }
    
    return word;
  }
  
  boolean alphabet[];
  boolean pre_alphabet[];
  boolean main_alphabet[];
  
  private void setAlphabets(int alph_size) {
    alphabet = new boolean[alph_size];
    pre_alphabet = new boolean[alph_size];
    main_alphabet = new boolean[alph_size];
    for (int i=0; i<word.length; i++) {
      alphabet[word[i]] = true;
      if (i < pre.length)
        pre_alphabet[word[i]] = true;
      else
        main_alphabet[word[i]] = true;
    }
  }
  
  private String toString = null;
  
  public String toString() {
    if (toString == null) {
      StringBuilder sb = new StringBuilder();
      
      for (short e : pre) {
        sb.append(e);
        sb.append(' ');
      }
      sb.append(" --> ");
      for (short e : main) {
        sb.append(e);
        sb.append(' ');
      }
      toString = sb.toString();
    }
    return toString;
  }
  
  public boolean implies(SScenario other) {
    
    if ( this.pre.length > other.pre.length && this.main.length < other.main.length) return false;
    
    if (   (isSubSequence(this.pre, other.pre, this.pre_alphabet) && isSubSequence(other.main, this.main, other.main_alphabet))
        || (!isSubSequence(other.pre, this.pre, other.pre_alphabet) && isSubSequence(other.word, this.main, other.alphabet))   )
    {
      return true;
    } else { 
      return false;
    }
  }
  
  public boolean subsumes(SScenario other) {
    return isSubSequence(this.pre, other.pre, this.pre_alphabet) && isSubSequence(other.main, this.main, other.main_alphabet);
  }
  
  public static boolean isSubSequence(short[] short_seq, short[] long_seq, boolean[] visible_in_short) {
    
    if (short_seq.length > long_seq.length) return false;
    
    int l=0;
    int s=0;
    while (s<short_seq.length) {
      
      // for each letter 'short_seq[s]' find a letter 'long_seq[l]'
      // starting at the last position
      while (l < long_seq.length) {
        if (short_seq[s] == long_seq[l]) {
          // short_seq[s] has a corresponding letter in other at position 'l'
          break;
        } else {
          // while searching for short_seq[s] we found another visible letter
          // first: short_seq[s] is not a subsequence
          if (visible_in_short[long_seq[l]])
            return false;
        }
        l++;
      }
      if (l == long_seq.length) {
        // could not find this[t] in other at any position
        return false;
      }
      l++; // move to next letter of 'other'
      s++;
    }
    return true; // first letter of long_seq[] after the last letter of short_seq[]
  }
  
  public static short[] getWord(String word) {
    String[] letters = word.split(",");
    
    short[] _word = new short[letters.length];
    for (int e=0; e<_word.length; e++) {
      _word[e] = Short.parseShort(letters[e]);
    }
    return _word;
  }
  
  public static SScenario getScenario(String scenario, int alphabet_size) {
    
    String[] charts = scenario.split("---");
    return new SScenario(getWord(charts[0]), getWord(charts[1]), alphabet_size);
  }

  public static void main(String []args) {
    SScenario s1 = new SScenario(new short[] {5,  184, 185, 186, 187, 115}, new short[] {197, 189, 190, 191}, 198);
    SScenario s2 = new SScenario(new short[] {34, 184, 185, 186, 187, 115}, new short[] {197, 189, 190, 191}, 198);
    
    s1.implies(s2);
  }
}
