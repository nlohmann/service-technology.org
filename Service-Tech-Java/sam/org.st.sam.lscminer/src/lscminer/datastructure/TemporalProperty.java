package lscminer.datastructure;

/**
 * A data structure captures temporal property of the form
 * {pre0,pre1,...} -> {post0,post1,...}
 * @author Anh Cuong Nguyen
 */
public class TemporalProperty {
  Integer[] preConditions;
  Integer[] postConditions;
  int support;
  double confidence;

  public TemporalProperty(Integer[] preConditions, Integer[] postConditions,int support,double confidence){
    this.preConditions = preConditions;
    this.postConditions = postConditions;
    this.support = support;
    this.confidence = confidence;
  }

  public Integer[] getPreConditions(){
    return this.preConditions;
  }
  
  public Integer[] getPostConditions(){
    return this.postConditions;
  }

  public int getSupport(){
    return support;
  }

  public double getConfidence(){
    return confidence;
  }

  @Override
  public String toString(){
    String property = "";
    for (int pre: preConditions){
      property += pre + " ";
    }
    property += "-> ";
    for (int post: postConditions){
      property += post + " ";
    }
    property += "(" + support + ", " + confidence + ")";

    return property;
  }

  /**
   * A temporal property is called stronger than another property, if 
   * i) they have the same support
   * ii) from the first property we can infer the other property. Formally, a
   * temporal property pre01 -> post01 is stronger than pre02 -> post02 if and
   * only if pre01 is a subsequence of pre02 and post01 is a super sequence of post02.
   * 
   * @param property
   * @return
   */
  public boolean isStronger(TemporalProperty property){
    return isSubsequence(this.preConditions, property.preConditions)
            && isSubsequence(property.postConditions, this.postConditions)
            && this.support >= property.support
            && this.confidence >= property.confidence;
  }

  /**
   * Check whether the first list is a subsequence of the second list
   * @param smallList the small list
   * @param bigList the big list
   * @return true if the small list is a subsequence of the second list; false otherwise
   */
  private boolean isSubsequence(Integer[] smallList, Integer[] bigList){
    int bigCurrentInx = 0;
    for (int smallInx = 0; smallInx < smallList.length; smallInx++){
      int event = smallList[smallInx];
      for (int inx = bigCurrentInx; inx < bigList.length; inx++){
        if (bigList[inx] == event){
          bigCurrentInx = inx+1;
          break;
        } else if (inx == bigList.length-1) {
          return false;
        }
      }

      if (smallInx == smallList.length-1){
        return true;
      } else if (bigCurrentInx >= bigList.length){
        return false;
      }
    }

    return false;
  }
}
