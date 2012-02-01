package hub.top.uma;

import java.util.HashMap;
import java.util.Map;

public class DNodeEmbeddingVisitor {
  
  private Map<DNode, DNode> embedding;
  
  public DNodeEmbeddingVisitor() {
    this(new HashMap<DNode, DNode>());
  }

  public DNodeEmbeddingVisitor(Map<DNode, DNode> embedding) {
    this.embedding = embedding;
  }

  public boolean canBeExtended(DNode toEmbed, DNode embedIn) {
    // check whether this node was embedded before (as predecessor of another node) 
    // and whether the embedding would be the same
    for (Map.Entry<DNode, DNode> m : embedding.entrySet()) {
      System.out.println(m.getKey()+" --> "+m.getValue());
    }
    System.out.println("trying "+toEmbed+" --> "+embedIn);
    if (embedding.containsKey(toEmbed) && embedding.get(toEmbed) != embedIn) {
      System.out.println("does not match");
      return false;
    }
    return true;
  }

  public void extend(DNode toEmbed, DNode embedIn) {
    embedding.put(toEmbed, embedIn);
  }

  public Map<DNode, DNode> getEmbedding() {
    return embedding;
  }
}
