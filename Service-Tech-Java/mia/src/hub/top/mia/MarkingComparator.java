package hub.top.mia;

import java.util.Comparator;

public class MarkingComparator implements Comparator<SafeMarking>{

	@Override
	public int compare(SafeMarking o1, SafeMarking o2) {
		int sum_o1 = o1.getM_Old().cardinality() + o1.getM_New().cardinality();
		int sum_o2= o2.getM_Old().cardinality() + o2.getM_New().cardinality();

		if (sum_o1 < sum_o2)  {
			return 1;
		}
		return 0;
	}
}
