package lscminer.datastructure;

/**
 * Represent an element in the database. The purpose is so that
 * it can be represented by pointer reference rather than value
 * type.
 *
 * @author Anh Cuong Nguyen
 */
public class SingleEvent {
    private int value;

    public SingleEvent(int value){
        this.value = value;
    }

    public int getValue(){
        return value;
    }

    public void setValue(int value){
        this.value = value;
    }

}
